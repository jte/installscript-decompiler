#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "Type.h"

class ScriptPrototype
{
public:
	enum class Flags : uint8_t
	{
		IsDLLFunction = 1,
		IsInternalFunction = 2,
		IsPredefinedFunction = 4,
		IsExported = 8,
		HasStdcallConvention = 0x10,
		IsGetProperty = 0x20,
		IsSetProperty = 0x40,
		HasVariableArgs = 0x80
	};
	void SetFlags(uint8_t flags)
	{
		m_flags = flags;
	}
	void SetArguments(std::vector<ArgumentTypeInfo> arguments)
	{
		m_arguments = arguments;
	}
	void SetName(const std::string& name)
	{
		m_name = name;
	}
	void SetReturnType(ScriptType returnType)
	{
		m_returnType = returnType;
	}
	void SetBBId(uint16_t bbId)
	{
		m_bbId = bbId;
	}
	uint16_t GetBBId() const
	{
		return m_bbId;
	}
	const std::string GetName() const
	{
		return m_name;
	}
	ScriptType GetReturnType() const
	{
		return m_returnType;
	}
	std::vector<ArgumentTypeInfo> GetArguments() const
	{
		return m_arguments;
	}
	bool GetIsExported() const
	{
		return m_flags & (uint8_t)Flags::IsExported;
	}
	friend std::ostream& operator<<(std::ostream& out, const ScriptPrototype& o);
	friend std::ostream& operator<<(std::ostream& out, const std::vector<ArgumentTypeInfo>& arguments);
protected:
	uint8_t m_flags; // CPrototype::Flags
	std::vector<ArgumentTypeInfo> m_arguments;
	std::string m_name;
	ScriptType m_returnType;
	uint16_t m_bbId;
};
