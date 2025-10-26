#pragma once

#include "Utils/AllocationPool.h"
#include <map>

template<class T> class MutualReference;
template<class T> class OwnerRefList;
template<class T> class OwnerRefSet;

template<class T>
class RefList
{
	using RefType = MutualReference<T>;

	RefType * m_head;
	RefType * m_tail;

public:

	class Iterator
	{
		RefType * m_ref;

		friend class RefList<T>;

		Iterator() : m_ref(0) {}
		Iterator(RefType * ref) : m_ref(ref) {}

	public:

		T * operator *() { return *m_ref; }

		void operator++() { m_ref = m_ref->m_next; }
		void operator++(int) { m_ref = m_ref->m_next; }

		void operator--() { if(m_ref->m_prev) m_ref = m_ref->m_prev; }
		void operator--(int) { if(m_ref->m_prev) m_ref = m_ref->m_prev; }

		bool operator==(Iterator & it) const { return it.m_ref == m_ref; }
		bool operator!=(Iterator & it) const { return !(it.m_ref == m_ref); }
	};

private:
	void append(RefType * ref)
	{
		if(!m_head)
		{
			m_head = ref;
			ref->m_prev = nullptr;

		}
		else
		{
			m_tail->m_next = ref;
			ref->m_prev = m_tail;
		}

		ref->m_next = nullptr;
		m_tail = ref;
	}

	void remove(RefType * ref)
	{
		RefType * next = ref->m_next;
		RefType * prev = ref->m_prev;

		if(prev) prev->m_next = next;
		else m_head = next;

		if(next) next->m_prev = prev;
		else m_tail = prev;
	}

public:
	RefList() : m_head(nullptr), m_tail(nullptr) {}

	Iterator begin() const
	{
		return Iterator(m_head);
	}

	Iterator end() const
	{
		return Iterator(nullptr);
	}

	friend class OwnerRefList<T>;
	friend class OwnerRefSet<T>;
};

template<class T>
class OwnerRefList
{
	using RefType = MutualReference<T>;

	AllocationPool<RefType> m_allocPool;
	RefType * m_head;

public:

	class Iterator
	{
		RefType * m_ref;

		friend class OwnerRefList<T>;

		Iterator() : m_ref(0) {}
		Iterator(RefType * ref) : m_ref(ref) {}

	public:

		T * operator *() { return *m_ref; }

		void operator++() { m_ref = m_ref->m_onext; }
		void operator++(int) { m_ref = m_ref->m_onext; }

		bool operator==(Iterator & it) const { return it.m_ref == m_ref; }
		bool operator!=(Iterator & it) const { return !(it.m_ref == m_ref); }
	};

public:
	OwnerRefList() : m_head(nullptr) {}

	Iterator begin() const
	{
		return Iterator(m_head);
	}

	Iterator end() const
	{
		return Iterator(nullptr);
	}

	void append(RefList<T>& list, T * object)
	{
		RefType * newref = m_allocPool.allocate(object, list);

		list.append(newref);

		newref->m_onext = m_head;
		m_head = newref;
	}

	void clear()
	{
		RefType * ref = m_head;

		while(ref)
		{
			RefType * onext = ref->m_onext;

			RefList<T>& list = ref->m_refList;
			list.remove(ref);

			ref = onext;
		}

		m_allocPool.clear();
		m_head = nullptr;
	}
};

template<class T>
class OwnerRefSet
{
	using RefType = MutualReference<T>;
	using ListType = RefList<T>*;

	AllocationPool<RefType> m_allocPool;
	std::map<ListType, RefType*> m_ownerSet;

public:
	OwnerRefSet() {}

	void append(RefList<T>& list, T* object)
	{
		if (m_ownerSet.count(&list) == 0)
		{
			RefType* newref = m_allocPool.allocate(object, list);

			list.append(newref);
			m_ownerSet[&list] = newref;
		}
	}

	void clear()
	{
		for (auto& owner : m_ownerSet)
		{
			RefType* ref = owner.second;

			RefList<T>& list = ref->m_refList;
			list.remove(ref);
		}

		m_allocPool.clear();
		m_ownerSet.clear();
	}
};

template<class T>
class MutualReference
{
	using RefType = MutualReference<T>;

	//List chain
	RefType * m_next;
	RefType * m_prev;

	//Owner list chain
	RefType * m_onext;
	RefType * m_oprev;

	T * m_object;
	RefList<T>& m_refList;

	MutualReference(T * object, RefList<T>& list)
	: m_next(nullptr)
	, m_prev(nullptr)
	, m_onext(nullptr)
	, m_oprev(nullptr)
	, m_object(object)
	, m_refList(list)
	{
	}

public:
	operator T*() { return m_object; }

	friend class RefList<T>;
	friend class OwnerRefList<T>;
	friend class OwnerRefSet<T>;
	friend class AllocationPool<RefType>;
};
