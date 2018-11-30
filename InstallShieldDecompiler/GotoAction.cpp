#include "GotoAction.h"
#include <cassert>
#include <string>
#include "CNumConst.h"
#include "Variable.h"

void CGotoAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 1);
	CNumConst* offset = dynamic_cast<CNumConst*>(m_arguments[0]);
	int32_t labelOffset = static_cast<int32_t>(offset->GetConst());
	os << "goto " << std::dec << labelOffset << std::hex << ';';
}

std::shared_ptr<CStatement> CGotoAction::ToStatement() const
{
	return std::make_shared<CStatement>(StatementType::Goto, CVariable::FromScript(m_arguments));
}