#pragma once

#include "stdio.h"
#include "stdint.h"
#include <string>

class FileStream
{
public:

	enum class Mode
	{
		Read,
		Write,
	};

private:

	FILE * m_file = nullptr;

public:

	FileStream()
	: m_file(nullptr)
	{
	}

	~FileStream()
	{
		if(m_file) fclose(m_file);
	}

	void open(const char * filename, Mode mode = Mode::Read)
	{
		errno_t error = fopen_s(&m_file, filename, (mode == Mode::Read) ? "rb" : "wb");
	}

	void close()
	{
		fclose(m_file);
		m_file = nullptr;
	}

	void moveto(long offset)
	{
		fseek(m_file, offset, SEEK_SET);
	}

	void move(long offset)
	{
		fseek(m_file, offset, SEEK_CUR);
	}

	bool is_open()
	{
		return m_file != nullptr;
	}

	bool eof()
	{
		return feof(m_file);
	}

	template<class T>
	FileStream & operator >> (T & value)
	{
		fread(&value, sizeof(T), 1, m_file);
		return *this;
	}

	template<class T>
	FileStream & operator << (const T & value)
	{
		fwrite(&value, sizeof(T), 1, m_file);
		return *this;
	}

	FileStream & operator >> (std::string& value)
	{
		uint32_t len;

		fread(&len, sizeof(uint32_t), 1, m_file);

		value.resize(len);

		fread(&value[0], 1, len, m_file);
		return *this;
	}

	FileStream & operator << (const std::string& value)
	{
		uint32_t len = value.size();

		fwrite(&len, sizeof(uint32_t), 1, m_file);
		fwrite(value.c_str(), 1, len, m_file);
		return *this;
	}
};
