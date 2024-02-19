#pragma once

#include <string>
#include <sstream>

enum class EVariableType
{
	Number,
	String,
	Variant
};

class CVariable
{
public:
	CVariable(int address, std::string name, EVariableType type) :
		m_address(address), m_name(name), m_isGlobal(address > -100), m_type(type)
	{
	}
	CVariable(int address, EVariableType type) :
		CVariable(address, GetNameForVariable(address, type), type)
	{

	}
	int GetAddress() const
	{
		return m_address;
	}
	std::string GetName() const
	{
		return m_name;
	}
	EVariableType GetType() const
	{
		return m_type;
	}
	bool IsGlobal() const
	{
		return m_isGlobal;
	}
	bool IsLocal() const
	{
		return !m_isGlobal;
	}
	void SetName(const std::string& name)
	{
		m_name = name;
	}
	static std::string GetNameForVariable(int address, EVariableType type)
	{
		std::stringstream ss;

		ss << ((address <= -100) ? "Lcl" : "Gbl");
		ss << "Var";
		//ss << (parameter ? "Param" : "Var");
		switch (type)
		{
		case EVariableType::String:
			ss << "Str";
			break;
		case EVariableType::Number:
			ss << "Num";
			break;
		case EVariableType::Variant:
			ss << "Obj";
			break;
		}
		ss << (address <= -100 ? (address + 100) * -1 : address);
		return ss.str();
	}
	friend std::ostream& operator<<(std::ostream& out, const CVariable& o);
protected:
	virtual void print(std::ostream& os) const = 0;
private:
	bool m_isGlobal;
	int m_address;
	std::string m_name;
	EVariableType m_type;
};