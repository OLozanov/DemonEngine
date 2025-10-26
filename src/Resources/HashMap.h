#pragma once

#include <string>

template<class T> class HashMap
{
	struct MapNode
	{
		std::string name;
		T * obj;

		MapNode * next;
		MapNode * prev;

		~MapNode()
		{
			delete obj;
		}
	};

	static constexpr int defn = 100;
	static constexpr int frate = 5;

	int m_tnum;
	MapNode ** m_table;

	int m_onum;

	unsigned long HashFunc(const std::string& name)
	{
		unsigned long hash = 0;

		const char * chr = name.c_str();
		while(*chr) hash += (hash << 5) + *chr++;

		return hash % m_tnum;
	}

	void addNode(int n, MapNode * node)
	{
		if (m_table[n]) m_table[n]->prev = node;

		node->next = m_table[n];
		node->prev = 0;

		m_table[n] = node;
	}

	void removeNode(int n, MapNode * entry)
	{
		if(m_table[n] == entry)
		{
			m_table[n] = entry->next;
			if (m_table[n]) m_table[n]->prev = 0;
		}
		else
		{
			MapNode * next = entry->next;
			MapNode * prev = entry->prev;

            prev->next = next;

            if(next) next->prev = prev;
		}
	}

	void resize()
	{
		int old_tnum = m_tnum;
		MapNode ** old_table = m_table;

		m_tnum *= 2;
		m_table = new MapNode*[m_tnum];

		for(int e = 0; e < old_tnum; e++)
		{
            MapNode * entry = old_table[e];

            while(entry)
			{
				int n = HashFunc(entry->name);
				addNode(n, entry);

				entry = entry->next;
			}
		}

		delete [] old_table;
	}

public:

	explicit HashMap(int n = defn)
	{
		m_tnum = n;
		m_table = new MapNode*[m_tnum];

		for(int i = 0; i < m_tnum; i++) m_table[i] = 0;

		m_onum = 0;
	}

	T * operator[](const std::string& name)
	{
		MapNode * entry = m_table[HashFunc(name)];

		while(entry)
		{
			if(entry->name == name) return entry->obj;
			entry = entry->next;
		}

		return nullptr;
	}

	bool exist(const std::string& name)
	{
		MapNode* entry = m_table[HashFunc(name)];

		while (entry)
		{
			if (entry->name == name) return true;
			entry = entry->next;
		}

		return false;
	}

	void add(const std::string& name, T * obj)
	{
		if(!obj) return;

		if(float(m_onum)/float(m_tnum) > frate) resize();

		int n = HashFunc(name);

		MapNode * newobj = new MapNode;

		newobj->name = name;
		newobj->obj = obj;

		addNode(n, newobj);

		m_onum++;
	}

	void remove(const std::string& name)
	{
		int n = HashFunc(name);
		MapNode * entry = m_table[n];

		while(entry)
		{
			if(entry->name == name)
			{
				removeNode(n, entry);
				delete entry;

				break;
			}

			entry = entry->next;
		}

		m_onum--;
	}

	void cleanup()
	{
		for(int e = 0; e < m_tnum; e++)
		{
            MapNode * entry = m_table[e];

            while(entry)
			{
				MapNode * next = entry->next;

				if(entry->obj->referenceCount() == 0)
				{
					removeNode(e, entry);
					delete entry;
				}

				entry = next;
			}
		}
	}
};
