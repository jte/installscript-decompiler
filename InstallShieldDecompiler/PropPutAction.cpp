#include "PropPutAction.h"
#include <string>
#include "MemberAccessExpression.h"
#include "CStrConst.h"

void CPropPutAction::print(std::ostream& os) const
{
	os << "Put " << *(m_arguments[0]) << " " << *(m_arguments[1]) << " " << *(m_arguments[2]);
}

std::shared_ptr<CStatement> CPropPutAction::ToStatement() const
{
	std::vector<CExpression*> exprs;
	CVariable* base = dynamic_cast<CVariable*>(m_arguments[0]->ToExpression());
	CStrConst* str = dynamic_cast<CStrConst*>(m_arguments[1]);
	CMemberAccessExpression* memb = new CMemberAccessExpression(base, str->GetString(), 0);
	exprs.push_back(memb);
	exprs.push_back(m_arguments[2]->ToExpression());
	return std::make_shared<CStatement>(StatementType::Assign, exprs);
}