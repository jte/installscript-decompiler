#include "SetByteAction.h"
#include <cassert>
#include <string>

void CSetByteAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

CStatement CSetByteAction::ToStatement() const
{
	return CStatement(StatementType::SetByte, CExpression::FromScript(m_arguments));
}