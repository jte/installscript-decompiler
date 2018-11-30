#pragma once

#include "Expression.h"
#include <string>

class CVariable;

class CArrayAccessExpression : public CExpression
{
public:
	enum class AccessType
	{
		StringArray,
		Array
	};
private:
	CVariable* m_base;
	size_t m_index;
	AccessType m_type;
public:
	CArrayAccessExpression(CVariable* base, size_t index, AccessType type) :
		m_base(base), m_index(index), m_type(type)
	{}
	~CArrayAccessExpression()
	{}
	void print(std::ostream& out);
};