#include "PropGetAction.h"
#include <cassert>
#include <string>

void CPropGetAction::print(std::ostream& os) const
{
	os << "Get " << *(m_arguments[0]) << " " << *(m_arguments[1]) << " " << *(m_arguments[2]);
}

std::shared_ptr<CStatement> CPropGetAction::ToStatement() const
{
	return std::make_shared<CStatement>(StatementType::PropGet, CExpression::FromScript(m_arguments));
}