#include "GetByteAction.h"
#include <cassert>
#include <string>

void CGetByteAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

CStatement CGetByteAction::ToStatement() const
{
	return CStatement(StatementType::Return, CExpression::FromScript(m_arguments));
}