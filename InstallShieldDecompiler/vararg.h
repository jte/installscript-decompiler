#pragma once

#include "IArgument.h"
#include "StreamPtr.h"
#include "Variable.h"
#include "Expression.h"


template <ArgType type, bool parameter>
class vararg : public IArgument
{
protected:
	int16_t m_address;
	void print(std::ostream& os) const
	{
		os << (parameter ? "p" : "v") << ' ' << type << ' ' << m_address;
	}
	
public:
	CExpression * ToExpression() const
	{
		return new CVariable(m_type, m_address, parameter);
	}
	vararg(StreamPtr& filePtr)
	{
		Parse(filePtr);
		m_type = type;
	}
	void Parse(StreamPtr& filePtr)
	{
		filePtr.Read(m_address);
		if (static_cast<int16_t>(m_address) <= -100)
		{
			m_address = static_cast<uint16_t>(m_address + 100);
		}
	}
	int16_t GetAddress() const
	{
		return m_address;
	}
};
