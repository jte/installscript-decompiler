#include "DataDeclList.h"

void CDataDeclList::Parse(StreamPtr& ptr)
{
	ptr.Read(m_numNumbers);
	
	ParseObjectTable(ptr);

	ptr.Read(m_numStrings);

	ParseStringTable(ptr);
}

void CDataDeclList::ParseObjectTable(StreamPtr& ptr)
{
	uint16_t numObjects;
	ptr.Read(numObjects);
	ptr.ReadArray(m_objectTable, numObjects);
}

void CDataDeclList::ParseStringTable(StreamPtr& ptr)
{
	uint16_t numStringVars;
	ptr.Read(numStringVars);

	ptr.ReadArray(m_stringTable, numStringVars);
}

std::ostream& operator<<(std::ostream& out, const CDataDeclList& o)
{
	o.print(out);
	out << std::endl;
	return out;
}

void CDataDeclList::print(std::ostream& os) const
{
	os << "numNumbers: " << m_numNumbers << std::endl;
	os << "numStrings: " << m_numStrings << std::endl;

	size_t i = 0;
	for (auto obj : m_objectTable)
	{ 
		if (((int)obj.flags & (int)SymFlags::vbArray) != 0)
		{
			os << "[" << i++ << "]: " << " flags: " << std::hex << (int)obj.flags << " count: " << std::dec << obj.elemCount << std::endl;
		}
		else
		{
			os << "[" << i++ << "]: " << " flags: " << std::hex << (int)obj.flags << " structId: " << std::dec << obj.typedefId << std::endl;
		}
	}

	i = 0;
	for (auto str : m_stringTable)
	{
		os << "[" << i++ << "]: " << " varId: " << str.varId << " count: " << str.stringSize << std::endl;
	}
}

std::ostream& operator<<(std::ostream& os, const std::vector<uint32_t>& vec) 
{
	os << "[";
	for (size_t i = 0; i < vec.size(); ++i) {
		os << std::hex << vec[i];
		if (i != vec.size() - 1)
			os << ", ";
	}
	os << "]";
	return os;
}