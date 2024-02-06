#include "DataDeclList.h"

void CDataDeclList::Parse(StreamPtr& ptr)
{
	uint16_t info1;
	ptr.Read(info1);

	ParseObjectTable(ptr);

	uint16_t info2;
	ptr.Read(info2);

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