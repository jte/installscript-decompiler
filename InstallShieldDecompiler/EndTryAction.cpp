#include "EndTryAction.h"
#include <cassert>
#include <string>

void CEndTryAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

std::shared_ptr<CStatement> CEndTryAction::ToStatement() const
{
	return std::make_shared<CStatement>(StatementType::EndTry, CExpression::FromScript(m_arguments));
}