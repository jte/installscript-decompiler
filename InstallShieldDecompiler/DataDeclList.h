#pragma once

#include "StreamPtr.h"
#include <vector>

class CDataDeclList
{
public:
	void Parse(StreamPtr& ptr);
protected:
	void ParseVariantTable(StreamPtr& ptr);
	void ParseSymFlags(StreamPtr& ptr);
private:
	uint16_t m_numVariables;
	std::vector<uint32_t> m_variantTable;
	std::vector<uint32_t> m_symFlags;
};