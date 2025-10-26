#pragma once

#include <cstring>
#include <vector>
#include <utility>

template<class T>
class AllocationPool
{
	static constexpr size_t ExtendFactor = 2;

	std::vector<T*> m_blocks;
	size_t m_freeptr;

	void extend()
	{
		size_t oldsz = m_blocks.size();
		size_t newsz = oldsz*ExtendFactor;
		m_blocks.resize(newsz);

		for(int i = oldsz; i < newsz; i++) m_blocks[i] = reinterpret_cast<T*>(new char[sizeof(T)]);
	}

public:

	explicit AllocationPool(size_t num = 2)
	: m_freeptr(0)
	{
		m_blocks.resize(num);
		for(size_t i = 0; i < num; i++) m_blocks[i] = reinterpret_cast<T*>(new char[sizeof(T)]);
	}

	~AllocationPool()
	{
		for(int i = 0; i < m_blocks.size(); i++) delete m_blocks[i];
	}

	template<typename ... Arg>
	T * allocate(Arg&& ... args)
	{
		if(m_freeptr == m_blocks.size()) extend();

		T * block = m_blocks[m_freeptr];
		m_freeptr++;

        new(block) T(std::forward<Arg>(args)...);

		return block;
	}

	void clear()
	{
        m_freeptr = 0;
	}
};
