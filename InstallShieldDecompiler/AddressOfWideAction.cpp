#include "AddressOfWideAction.h"
#include <cassert>
#include <string>
#include "AddressOfExpression.h"

void CAddressOfWideAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

std::shared_ptr<CStatement> CAddressOfWideAction::ToStatement() const
{
	std::vector<CExpression*> exprs;
	exprs.push_back(m_arguments[0]->ToExpression());
	CVariable* var = dynamic_cast<CVariable*>(m_arguments[1]->ToExpression());
	CAddressOfExpression* addrOf = new CAddressOfExpression(var);
	exprs.push_back(addrOf);
	return std::make_shared<CStatement>(StatementType::Assign, exprs);
}

CAddressOfWideAction::CAddressOfWideAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}