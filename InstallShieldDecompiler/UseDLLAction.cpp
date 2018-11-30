#include "UseDLLAction.h"
#include <cassert>
#include <string>

void CUseDLLAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

std::shared_ptr<CStatement> CUseDLLAction::ToStatement() const
{
	return std::make_shared<CStatement>(StatementType::UseDLL, CExpression::FromScript(m_arguments));
}