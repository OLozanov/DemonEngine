#pragma once

#include <tuple>
#include <deque>
#include <utility>

//Deferred calls
class IAsyncCall
{
public:

	virtual ~IAsyncCall() {}
	virtual void invoke() = 0;
};

class AsyncCall
{
public:

	AsyncCall(IAsyncCall * call)
		: m_call(call)
	{
	}

	~AsyncCall()
	{
		delete m_call;
	}

	void operator()() const
	{
		m_call->invoke();
	}

private:

	IAsyncCall * m_call;
};

template<int ...> struct Sequence
{};

template<int N, int ...S> struct SeqGen : SeqGen<N-1, N-1, S...>
{};

template<int ...S> struct SeqGen<0, S...>
{
	typedef Sequence<S...> type;
};

template <typename T, typename ... Arg>
class AsyncObjectCall : public IAsyncCall
{
	typedef std::tuple<typename std::remove_reference<Arg>::type...> ArgHolder;

private:

	template<class C> struct Invoker;

	template<int ...S>
	struct Invoker<Sequence<S...>>
	{
		static void invoke(T& handler, ArgHolder& arguments)
		{
			handler(std::get<S>(arguments)...);
		}
	};

private:

	T m_handler;
	ArgHolder m_arguments;

public:

	AsyncObjectCall(const T& obj, Arg ... args)
	: m_handler(obj)
	, m_arguments(args...)
	{}

	~AsyncObjectCall() {}

	void invoke() override
	{
		Invoker<typename SeqGen<sizeof...(Arg)>::type>::invoke(m_handler, m_arguments);
	}
};

template <typename T, typename ... Arg>
class AsyncMethodCall : public IAsyncCall
{
public:
	typedef void (T::*MethodPtr)(Arg ... args);
	typedef std::tuple<typename std::remove_reference<Arg>::type...> ArgHolder;

private:

	template<class C> struct Invoker;

	template<int ...S>
	struct Invoker<Sequence<S...>>
	{
		static void invoke(T * receiver, MethodPtr& handler, ArgHolder& arguments)
		{
			(receiver->*handler)(std::get<S>(arguments)...);
		}
	};

private:

	T * m_receiver;
	MethodPtr m_handler;
	ArgHolder m_arguments;

public:

	AsyncMethodCall(T * obj, MethodPtr func, Arg ... args)
	: m_receiver(obj)
	, m_handler(func)
	, m_arguments(args...)
	{}

	~AsyncMethodCall() {}

	void invoke()
	{
		Invoker<typename SeqGen<sizeof...(Arg)>::type>::invoke(m_receiver, m_handler, m_arguments);
	}
};

//
class AsyncCallQueue
{
public:

	template <typename T, typename ... Arg>
	void invoke(const T& obj, Arg&& ... args)
	{
		IAsyncCall* call = new AsyncObjectCall<T, Arg...>(obj, std::forward<Arg>(args)...);
		m_callQueue.emplace_back(call);
	}

	template <typename T, typename ... Arg>
	void invoke(T * obj, typename AsyncMethodCall<T, Arg...>::MethodPtr fptr, Arg&& ... args)
	{
		IAsyncCall* call = new AsyncMethodCall<T, Arg...>(obj, fptr, std::forward<Arg>(args)...);
		m_callQueue.emplace_back(call);
	}

	void execute()
	{
		while(!m_callQueue.empty())
		{
            const auto& call = m_callQueue.front();
            call();
            m_callQueue.pop_front();
		}
	}

private:

	std::deque<AsyncCall> m_callQueue;
};
