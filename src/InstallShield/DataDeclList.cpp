#include "DataDeclList.h"

void CDataDeclList::Parse(StreamPtr& ptr)
{
	ptr.Read(m_numVariables);
	ParseVariantTable(ptr);
	ParseSymFlags(ptr);
}

void CDataDeclList::ParseVariantTable(StreamPtr& ptr)
{
	uint16_t numVariants;
	ptr.Read(numVariants);
	ptr.ReadArray(m_variantTable, numVariants);
}

void CDataDeclList::ParseSymFlags(StreamPtr& ptr)
{
	uint16_t num;
	ptr.Read(num);
	uint16_t unknown;
	ptr.Read(unknown);
	uint16_t numEntries;
	ptr.Read(numEntries);
	ptr.ReadArray(m_symFlags, numEntries);
}

std::ostream& operator<<(std::ostream& out, const CDataDeclList& o)
{
	o.print(out);
	out << std::endl;
	return out;
}

void CDataDeclList::print(std::ostream& os) const
{
	os << "DataDeclList START" << std::endl;
	os << "numVariables: " << m_numVariables << std::endl;
	os << "variantTable START" << std::endl;
	os << m_variantTable << std::endl;
	os << "variantTable END" << std::endl;
	os << "symFlags START" << std::endl;
	os << m_symFlags << std::endl;
	os << "symFlags END" << std::endl;
	os << "DataDeclList END" << std::endl;
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