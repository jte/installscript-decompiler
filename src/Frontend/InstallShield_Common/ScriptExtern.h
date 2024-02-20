#pragma once

#include <string>
#include <cstdint>

enum class EScriptExternType
{
	Variant
};

class ScriptExtern
{
public:
	ScriptExtern(uint8_t type, std::string name, int16_t address) :
		m_name(name), m_address(address)
	{
		if (type == 1)
		{
			m_type = EScriptExternType::Variant;
		}
	}
	void SetName(const std::string& name)
	{
		m_name = name;
	}
	void SetAddress(int16_t address)
	{
		m_address = address;
	}
	void SetType(EScriptExternType type)
	{
		m_type = type;
	}
	const std::string GetName() const
	{
		return m_name;
	}
	int16_t GetAddress() const
	{
		return m_address;
	}
	EScriptExternType GetType() const
	{
		return m_type;
	}
protected:
private:
	std::string m_name;
	int16_t m_address;
	EScriptExternType m_type;
};
