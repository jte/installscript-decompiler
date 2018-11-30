#include "BindVariableAction.h"
#include <cassert>
#include <string>

void CBindVariableAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

std::shared_ptr<CStatement> CBindVariableAction::ToStatement() const
{
	return std::make_shared<CStatement>(StatementType::BindVariable, CExpression::FromScript(m_arguments));
}