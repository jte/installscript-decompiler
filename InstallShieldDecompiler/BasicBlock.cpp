#include "BasicBlock.h"

std::ostream& operator<<(std::ostream& out, const CBasicBlock& o)
{
	for (auto& s : o.m_statements)
	{
		out << s;
	}
	return out;
}