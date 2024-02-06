#include "Struct.h"
#include "IScript.h"
#include <ostream>

void CStruct::Parse(StreamPtr& filePtr)
{
	if (m_script->GetHeaderKind() == HeaderKind::OBS) {
		m_name = filePtr.ReadInsString();
	}

	uint16_t numMembers = 0;
	filePtr.Read(numMembers);
	m_members.resize(numMembers);

	for (auto& member : m_members)
	{
		filePtr.Read(member.type);
		filePtr.Read(member.repCount);
		member.name = filePtr.ReadInsString();
	}
}

const std::string CStruct::GetName() const
{
	return m_name;
}

std::ostream& operator<<(std::ostream& os, const CStruct& obj)
{
	// write obj to stream
	os << "typedef " << obj.m_name << '\n';
	os << "\tbegin\n";

	for (const auto& m : obj.m_members)
	{
		os << "\t\t";
		if (m.type == ScriptType::Struct)
		{
			os << obj.m_script->GetStruct(m.repCount).GetName() << " " << m.name;
		}
		else
		{
			os << m.type << " " << m.name;
			if (m.repCount > 1)
			{
				if (m.type == ScriptType::WString || m.type == ScriptType::String)
				{
					os << "[" << m.repCount << "]";
				}
				else
				{
					os << "(" << m.repCount << ")";
				}
			}
		}
		
		os << ";\n";
	}

	os << "\tend;" << std::endl;
	return os;
}
