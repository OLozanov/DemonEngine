#pragma once

#include "LinkedList.h"
#include <algorithm>

template <class T>
class EditList
{
	T* m_head;
	T* m_tail;

public:

	using ListType = EditList<T>;

	//Range-base loops support
	class Iterator
	{
		T* m_object;

		friend class EditList<T>;

		Iterator() : m_object(nullptr) {}
		Iterator(T* obj) : m_object(obj) {}

	public:

		T* operator *() { return m_object; }

		Iterator next() { return Iterator(m_object->snext); }
		Iterator prev() { return Iterator(m_object->sprev); }

		Iterator& operator++() { m_object = m_object->snext; return *this; }
		Iterator operator++(int) { Iterator it(*this); m_object = m_object->snext; return it; }

		Iterator& operator--() { if (m_object->sprev) m_object = m_object->sprev; return *this; }
		Iterator operator--(int) { Iterator it(*this); if (m_object->sprev) m_object = m_object->sprev; return it; }

		bool operator==(const Iterator& it) const { return it.m_object == m_object; }
		bool operator!=(const Iterator& it) const { return !(it.m_object == m_object); }
	};

public:

	EditList() : m_head(nullptr), m_tail(nullptr) {}

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

	T* head() { return m_head; }
	T* tail() { return m_tail; }

	void append_front(T* newobj)
	{
		newobj->snext = m_head;
		newobj->sprev = nullptr;

		if (m_head) m_head->sprev = newobj;
		else m_tail = newobj;

		m_head = newobj;
	}

	void append(T* newobj)
	{
		if (!m_head)
		{
			m_head = newobj;
			newobj->sprev = nullptr;
		}
		else
		{
			m_tail->snext = newobj;
			newobj->sprev = m_tail;
		}

		newobj->snext = nullptr;
		m_tail = newobj;
	}

	void remove(T* object)
	{
		T* onext = object->snext;
		T* oprev = object->sprev;

		if (oprev) oprev->snext = onext;
		else m_head = onext;

		if (onext) onext->sprev = oprev;
		else m_tail = oprev;
	}

	void merge(const ListType& second)
	{
		if (!m_head)
		{
			m_head = second.m_head;
			m_tail = second.m_tail;
		}
		else
		{
			m_tail->snext = second.m_head;
			m_tail = second.m_tail;
		}
	}

	void destroy()
	{
		T* obj = m_head;

		while (obj)
		{
			T* onext = obj->snext;
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
class EditListNode
{
private:
    T* next;
    T* prev;

    T* snext;
    T* sprev;

    friend class LinkedList<T>;
    friend class EditList<T>;
};