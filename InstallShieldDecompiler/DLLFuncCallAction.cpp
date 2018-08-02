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
	const CDLLPrototype* p = dynamic_cast<const CDLLPrototype*>(m_script->GetPrototype(m_functionId));
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

CStatement CDLLFuncCallAction::ToStatement() const
{
	return CStatement(StatementType::DLLFuncCall, 
		{ new CFunctionCallExpression(m_script->GetPrototype(m_functionId), CVariable::FromScript(m_arguments)) });
}