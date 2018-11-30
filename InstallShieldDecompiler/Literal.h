#pragma once

#include "Expression.h"
#include <string>

class CLiteral : public CExpression
{
public:
	~CLiteral()
	{}
};

class CStringLiteral : public CLiteral
{
private:
	const std::string m_value;
public:
	CStringLiteral(const std::string& value) :
		m_value(value)
	{
	}
	void print(std::ostream& out)
	{
		out << '"' << m_value << '"';
	}
	const std::string GetValue() const
	{
		return m_value;
	}
};

class CNumberLiteral : public CLiteral
{
private:
	const int32_t m_value;
public:
	CNumberLiteral(const int32_t value) :
		m_value(value)
	{
	}
	void print(std::ostream& out)
	{
		out << m_value;
	}
	const int32_t GetValue() const
	{
		return m_value;
	}
};