#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>
#include <ostream>

class StreamPtr
{
private:
	const uint8_t * m_base;
	size_t m_offset;
	size_t m_size;
public:
	StreamPtr(const std::vector<uint8_t>& base, size_t offset = 0) :
		m_base(&base[0]), m_offset(offset), m_size(base.size())
	{}
	void JumpTo(size_t offset)
	{
		m_offset = offset;
	}
	size_t GetOffset() const
	{
		return m_offset;
	}
	template <typename T>
	void Read(T& output, bool advance = true)
	{
		output = *reinterpret_cast<const T*>(m_base + m_offset);
		if (advance)
		{
			m_offset += sizeof(T);
			if (m_offset > m_size)
			{
				throw std::runtime_error("Tried to read beyond end of file.");
			}
		}
	}
	template <typename T>
	void ReadArray(std::vector<T>& output, size_t length)
	{
		output.reserve(length);
		for (size_t i = 0; i < length; ++i)
		{
			T elem;
			Read(elem, true);
			output.push_back(elem);
		}
	}
	std::string ReadInsString()
	{
		uint16_t length = 0;
		Read(length);
		std::vector<char> buffer;
		ReadArray(buffer, length);
		return std::string(buffer.begin(), buffer.end());
	}
	bool ReachedEnd() const
	{
		return m_offset == m_size;
	}
};

std::ostream& operator<<(std::ostream& os, const char* str);