#include "ScriptPrototype.h"
#include <ostream>

std::ostream& operator<<(std::ostream& out, const std::vector<ArgumentTypeInfo>& arguments)
{
	for (auto it = arguments.begin(); it != arguments.end(); ++it)
	{
		out << it->scriptType;
		if (it != arguments.end() - 1)
			out << ',';
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const ScriptPrototype& proto)
{
	if (proto.m_flags & (uint8_t)ScriptPrototype::Flags::IsExported)
	{
		out << "export ";
	}
	out << "prototype ";
	if (proto.m_bbId != 0xffff)
		out << proto.m_returnType << ' ';
	if (proto.m_flags & (uint8_t)ScriptPrototype::Flags::IsExported)
	{
		out << proto.m_name;
	}
	else
	{
		out << "predefined_func";
	}
	if (proto.m_arguments.size())
	{
		out << '(';
		out << proto.m_arguments;
		out << ");";
	}
	return out;
}