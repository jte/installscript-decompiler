#include "DLLFuncCallAction.h"
#include "Prototype/DLLPrototype.h"
#include "IScript.h"
#include <cassert>

CDLLFuncCallAction::CDLLFuncCallAction(CIScript* script, StreamPtr& filePtr) :
	CFuncCallAction(script, filePtr)
{
}

AbstractExpression* CDLLFuncCallAction::ToExpression() const
{
	const CDLLPrototype* p = dynamic_cast<const CDLLPrototype*>(m_script->GetFnById(m_functionId).prototype);
	auto name = p->GetName();
	auto flags = p->GetFlags();
	std::stringstream ss;
	if (((uint32_t)flags & (uint32_t)CPrototype::Flags::IsExported) || !name.empty())
		ss << p->GetDLLName() << '.' << name;
	else
		ss << "Func_" << p->GetBBId();

	std::vector<AbstractExpression*> args;
	for (const auto& arg : m_arguments)
	{
		args.push_back(arg->ToExpression());
	}

	return new AssignExpression(new VariableExpression("GblVarObj0"), new FunctionCallExpression(ss.str(), args));
}

void CDLLFuncCallAction::print(std::ostream& os) const
{
	const CDLLPrototype* p = dynamic_cast<const CDLLPrototype*>(m_script->GetFnById(m_functionId).prototype);
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
