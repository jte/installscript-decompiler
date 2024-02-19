#pragma once

#include "Variable.h"
#include <optional>
#include <cstdint>

class CStringVariable : public CVariable
{
public:
	CStringVariable(int address, std::optional<uint16_t> stringSize = std::nullopt) : 
		CVariable(address, EVariableType::String), m_stringSize(stringSize)
	{

	}
protected:
	void print(std::ostream& os) const override
	{
		os << "STRING " << GetName();
		if (m_stringSize.has_value())
		{
			os << "[" << m_stringSize.value() << "]";
		}
	}
private:
	std::optional<uint16_t> m_stringSize;
};