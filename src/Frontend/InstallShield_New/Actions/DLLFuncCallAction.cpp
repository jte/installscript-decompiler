#include "DLLFuncCallAction.h"
#include "Prototype/DLLPrototype.h"
#include "IScript.h"
#include <cassert>
#include "Variables/SymbolTable.h"
#include "ScriptPrototype.h"

namespace newis
{

CDLLFuncCallAction::CDLLFuncCallAction(CIScript* script, StreamPtr& filePtr) :
	CFuncCallAction(script, filePtr)
{
}

::AbstractExpression* CDLLFuncCallAction::ToExpression(::SymbolTable* symTable) const
{
	auto p = m_script->GetFnById(m_functionId).prototype;

	std::vector<::AbstractExpression*> args;
	for (const auto& arg : m_arguments)
	{
		args.push_back(arg->ToExpression(symTable));
	}

	return new AssignExpression(new VariableExpression(symTable->GetByName("LAST_RESULT", EVariableType::Variant, true)), new FunctionCallExpression(p->GetName(), args));
}

void CDLLFuncCallAction::print(std::ostream& os) const
{
	os << m_script->GetFnById(m_functionId).prototype->GetName();
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

};