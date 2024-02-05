#pragma once

#include "StreamPtr.h"
#include <vector>

class CDataDeclList
{
public:
	void Parse(StreamPtr& ptr);
	friend std::ostream& operator<<(std::ostream& out, const CDataDeclList& o);
protected:
	void ParseVariantTable(StreamPtr& ptr);
	void ParseSymFlags(StreamPtr& ptr);
	virtual void print(std::ostream& os) const;
private:
	uint16_t m_numVariables;
	std::vector<uint32_t> m_variantTable;
	std::vector<uint32_t> m_symFlags;
};

std::ostream& operator<<(std::ostream& os, const std::vector<uint32_t>& vec);