#include "TryAction.h"
#include <cassert>
#include <string>

void CTryAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

std::shared_ptr<CStatement> CTryAction::ToStatement() const
{
	return std::make_shared<CStatement>(StatementType::Try, CExpression::FromScript(m_arguments));
}