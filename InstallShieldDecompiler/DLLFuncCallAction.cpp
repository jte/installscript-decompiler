#include "DLLFuncCallAction.h"
#include "DLLPrototype.h"
#include "IScript.h"
#include <cassert>
#include "FunctionCall.h"

CDLLFuncCallAction::CDLLFuncCallAction(CIScript* script, StreamPtr& filePtr) :
	CFuncCallAction(script, filePtr)
{
}

void CDLLFuncCallAction::print(std::ostream& os) const
{
	const CDLLPrototype* p = dynamic_cast<const CDLLPrototype*>(m_script->GetFnByBBId(m_functionId).prototype);
	assert(p);
	auto name = p->GetName();
	auto flags = p->GetFlags();
	os << "dll: " << p->GetDLLName() << ";name:" << name;
	os << "(";
	for (const auto& arg : m_arguments)
	{
		os << *arg;
		if (arg != m_arguments.back())
		{
			os << ", ";
		}
	}
	os << ")";
}

std::shared_ptr<CStatement> CDLLFuncCallAction::ToStatement() const
{
	std::vector<CExpression*> exprs;
	auto fnCall = new CFunctionCallExpression(m_script->GetFnById(m_functionId).prototype, CVariable::FromScript(m_arguments));
	exprs.push_back(fnCall);
	return std::make_shared<CStatement>(StatementType::DLLFuncCall, exprs);
}