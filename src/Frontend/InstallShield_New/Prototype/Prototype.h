#pragma once

#include <cstdint>
#include <vector>
#include "StreamPtr.h"
#include "Type.h"

namespace newis
{

class CPrototype
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
protected:
	uint8_t m_flags; // CPrototype::Flags
	std::vector<ArgumentTypeInfo> m_arguments;
	std::string m_name;
	ScriptType m_returnType;
	uint16_t m_bbId;
	virtual void print(std::ostream& os) const;
public:
	CPrototype(StreamPtr& filePtr);
	virtual ~CPrototype() = default;
	virtual uint16_t GetBBId() const
	{
		return m_bbId;
	}
	virtual void Parse(StreamPtr& filePtr)
	{
		filePtr.Read(m_flags);
	}
	virtual void ParseArguments(StreamPtr& filePtr);
	std::string GetName() const;
	Flags GetFlags() const;
	ScriptType GetReturnType() const;
	bool GetIsExported() const;
	bool GetHasStdcallConvention() const;
	bool GetIsGetProperty() const;
	bool GetIsSetProperty() const;
	bool GetHasVariableArgs() const;
	std::vector<ArgumentTypeInfo> GetArgumentTypeInfo() const;

	typedef CPrototype* (*MakeFn)(StreamPtr& filePtr);
	static CPrototype::MakeFn FindFactory(size_t flags);
	friend std::ostream& operator<<(std::ostream& out, const CPrototype& o);
	friend std::ostream& operator<<(std::ostream& out, const std::vector<ArgumentTypeInfo>& arguments);
};

std::ostream& operator<<(std::ostream& out, const std::vector<ArgumentTypeInfo>& arguments);

};