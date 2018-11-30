#pragma once

#include "Expression.h"
#include <string>

class CVariable;

class CAddressOfExpression : public CExpression
{
private:
	CVariable* m_var;
public:
	CAddressOfExpression(CVariable* var) :
		m_var(var)
	{}
	~CAddressOfExpression()
	{}
	void print(std::ostream& out);
};