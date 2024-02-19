#include "ScriptStruct.h"
#include <ostream>
#include "Frontend.h"

std::ostream& operator<<(std::ostream& os, const ScriptStruct& obj)
{
	// write obj to stream
	os << "typedef " << obj.m_name << '\n';
	os << "\tbegin\n";

	for (const auto& m : obj.m_members)
	{
		os << "\t\t";
		if (m.type == ScriptType::Struct)
		{
			os << obj.m_frontend->GetStruct(m.repCount)->GetName() << " " << m.name;
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