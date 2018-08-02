#include "PropPutAction.h"
#include <string>

void CPropPutAction::print(std::ostream& os) const
{
	os << "Put " << *(m_arguments[0]) << " " << *(m_arguments[1]) << " " << *(m_arguments[2]);
}

CStatement CPropPutAction::ToStatement() const
{
	return CStatement(StatementType::PropPut, CExpression::FromScript(m_arguments));
}