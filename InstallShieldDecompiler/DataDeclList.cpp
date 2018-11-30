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