#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "Type.h"

class CFrontend;

class ScriptStruct
{
	
public:
	struct CStructMember
	{
		std::string name;
		uint16_t repCount;
		ScriptType type;

		CStructMember(std::string name, uint16_t repCount, ScriptType type) :
			name(name), repCount(repCount), type(type)
		{

		}
	};
	void SetName(const std::string& name)
	{
		m_name = name;
	}
	const std::string GetName() const
	{
		return m_name;
	}
	void SetFrontend(CFrontend* frontend)
	{
		m_frontend = frontend;
	}
	void AddMember(CStructMember member)
	{
		m_members.push_back(member);
	}
	friend std::ostream& operator<<(std::ostream& out, const ScriptStruct& o);
protected:
private:
	std::string m_name;
	std::vector<CStructMember> m_members;
	CFrontend* m_frontend;
};