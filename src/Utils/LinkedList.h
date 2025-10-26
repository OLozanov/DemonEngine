#pragma once

template <class T>
class LinkedList
{
	T * m_head;
	T * m_tail;

public:

	using ListType = LinkedList<T>;

	//Range-base loops support
	class Iterator
	{
		T * m_object;

		friend class LinkedList<T>;

		Iterator() : m_object(nullptr) {}
		Iterator(T * obj) : m_object(obj) {}

	public:

		T * operator *() { return m_object; }

		Iterator next() { return Iterator(m_object->next); }
		Iterator prev() { return Iterator(m_object->prev); }

		Iterator& operator++() { m_object = m_object->next; return *this; }
		Iterator operator++(int) { Iterator it(*this); m_object = m_object->next; return it; }

		Iterator& operator--() { if(m_object->prev) m_object = m_object->prev; return *this; }
		Iterator operator--(int) { Iterator it(*this); if(m_object->prev) m_object = m_object->prev; return it; }

		bool operator==(const Iterator& it) const { return it.m_object == m_object; }
		bool operator!=(const Iterator& it) const { return !(it.m_object == m_object); }
	};

	class ReverseIterator
	{
		T* m_object;

		friend class LinkedList<T>;

		ReverseIterator() : m_object(nullptr) {}
		ReverseIterator(T* obj) : m_object(obj) {}

	public:

		T* operator *() { return m_object; }

		ReverseIterator next() { return ReverseIterator(m_object->prev); }
		ReverseIterator prev() { return ReverseIterator(m_object->next); }

		ReverseIterator& operator++() { m_object = m_object->prev; return *this; }
		ReverseIterator operator++(int) { ReverseIterator it(*this); m_object = m_object->prev; return it; }

		ReverseIterator& operator--() { if (m_object->next) m_object = m_object->next; return *this; }
		ReverseIterator operator--(int) { ReverseIterator it(*this); if (m_object->next) m_object = m_object->next; return it; }

		bool operator==(const ReverseIterator& it) const { return it.m_object == m_object; }
		bool operator!=(const ReverseIterator& it) const { return !(it.m_object == m_object); }
	};

	class ReverseAdapter
	{
		const LinkedList& m_list;

	public:
		ReverseAdapter(const LinkedList& list) : m_list(list) {}

		auto begin() const { return m_list.rbegin(); }
		auto end() const { return m_list.rend(); }
	};

public:

	LinkedList() : m_head(nullptr), m_tail(nullptr) {}

	const ListType& operator=(const ListType& list)
	{
		m_head = list.m_head;
		m_tail = list.m_tail;

		return *this;
	}

	ListType& operator=(ListType&& list)
	{
		std::swap(m_head, list.m_head);
		std::swap(m_tail, list.m_tail);

		return *this;
	}

	Iterator begin() const
	{
		return Iterator(m_head);
	}

	Iterator end() const
	{
		return Iterator(nullptr);
	}

	ReverseIterator rbegin() const
	{
		return ReverseIterator(m_tail);
	}

	ReverseIterator rend() const
	{
		return ReverseIterator(nullptr);
	}

	ReverseAdapter reverse() { return ReverseAdapter(*this); }

	T * head() { return m_head; }
	T * tail() { return m_tail; }

	void append_front(T * newobj)
	{
		newobj->next = m_head;
		newobj->prev = nullptr;

		if(m_head) m_head->prev = newobj;
		else m_tail = newobj;

		m_head = newobj;
	}

	void append(T * newobj)
	{
		if(!m_head)
		{
			m_head = newobj;
			newobj->prev = nullptr;
		}
		else
		{
			m_tail->next = newobj;
			newobj->prev = m_tail;
		}

		newobj->next = nullptr;
		m_tail = newobj;
	}

	void remove(T * object)
	{
		T * onext = object->next;
		T * oprev = object->prev;

		if(oprev) oprev->next = onext;
		else m_head = onext;

		if(onext) onext->prev = oprev;
		else m_tail = oprev;
	}

	void merge(const ListType& second)
	{
		if(!m_head)
		{
			m_head = second.m_head;
			m_tail = second.m_tail;
		}
		else
		{
			m_tail->next = second.m_head;
			m_tail = second.m_tail;
		}
	}

	void destroy()
	{
        T * obj = m_head;

        while(obj)
		{
			T * onext = obj->next;
			delete obj;
			obj = onext;
		}

		m_head = 0;
		m_tail = 0;
	}

	void clear()
	{
	    m_head = 0;
	    m_tail = 0;
	}

	bool empty() { return (m_head == nullptr); }
};

template <class T>
class ListNode
{
private:
    T * next;
    T * prev;

    friend class LinkedList<T>;
};
