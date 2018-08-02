#include "PropGetAction.h"
#include <cassert>
#include <string>

void CPropGetAction::print(std::ostream& os) const
{
	os << "Get " << *(m_arguments[0]) << " " << *(m_arguments[1]) << " " << *(m_arguments[2]);
}

CStatement CPropGetAction::ToStatement() const
{
	return CStatement(StatementType::PropGet, CExpression::FromScript(m_arguments));
}