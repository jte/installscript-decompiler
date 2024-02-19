#pragma once

#include "Variable.h"
#include <optional>
#include "ScriptStruct.h"
#include <cstdint>

class CVariantVariable : public CVariable
{
public:
	CVariantVariable(int address) : 
		CVariable(address, EVariableType::Variant), 
		m_elemCount(std::nullopt), 
		m_struct(nullptr),
		m_isArray(false),
		m_isStruct(false)
	{

	}
	void SetElemCount(uint16_t elemCount)
	{
		m_elemCount = elemCount;
	}
	void SetTypedef(const ScriptStruct* pstruct)
	{
		m_struct = pstruct;
	}
	void SetIsArray(bool set)
	{
		m_isArray = set;
	}
	void SetIsStruct(bool set)
	{
		m_isStruct = set;
	}
protected:
	void print(std::ostream& os) const override
	{
		if (m_isArray)
		{
			os << "VARIANT " << GetName() << "(" << m_elemCount.value() << ")";
		}
		else if (m_isStruct)
		{
			os << m_struct->GetName() << " " << GetName();
		}
		else
		{
			os << "VARIANT " << GetName();
		}
	}
private:
	const ScriptStruct* m_struct;
	std::optional<uint16_t> m_elemCount;
	bool m_isArray;
	bool m_isStruct;
};