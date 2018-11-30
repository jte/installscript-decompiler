#pragma once

#include "Expression.h"
#include <string>

class CVariable;

class CMemberAccessExpression : public CExpression
{
private:
	CVariable* m_base;
	std::string m_byString;
	size_t m_byIndex;
public:
	CMemberAccessExpression(CVariable* base, std::string byString, size_t byIndex) :
		m_base(base), m_byString(byString), m_byIndex(byIndex)
	{}
	~CMemberAccessExpression()
	{}
	void print(std::ostream& out);
};