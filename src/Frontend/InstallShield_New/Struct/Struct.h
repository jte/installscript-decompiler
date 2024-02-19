#pragma once

#include "StreamPtr.h"
#include <string>
#include "Type.h"

namespace newis
{

class CIScript;

class CStruct
{
	struct CStructMember
	{
		std::string name;
		uint16_t repCount;
		ScriptType type;
	};
private:
	std::string m_name;
	std::vector<CStructMember> m_members;
	const CIScript * const m_script;
public:
	CStruct(const CIScript * const script) :
		m_script(script)
	{}
	const std::string GetName() const;
	const std::vector<CStructMember> GetMembers() const
	{
		return m_members;
	}
	void Parse(StreamPtr& filePtr);
	friend std::ostream& operator<<(std::ostream& out, const CStruct& o);
};

};