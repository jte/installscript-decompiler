#include "PropPutRefAction.h"
#include <string>

void CPropPutRefAction::print(std::ostream& os) const
{
	os << "PutRef " << *(m_arguments[0]) << " " << *(m_arguments[1]);
}

std::shared_ptr<CStatement> CPropPutRefAction::ToStatement() const
{
	return std::make_shared<CStatement>(StatementType::PropPutRef, CExpression::FromScript(m_arguments));
}