#include "InternalFuncCallAction.h"
#include "IScript.h"

namespace oldis
{

CInternalFuncCallAction::CInternalFuncCallAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{
	filePtr.Read(m_protoCode);
	filePtr.Read(m_PrototypeIndex);
	filePtr.Read(m_labelCode);
	filePtr.Read(m_eventIndex);
	size_t numArguments = m_script->GetFnById(m_PrototypeIndex).prototype->GetArguments().size();
	m_numOperands = numArguments;
	m_arguments.resize(m_numOperands);
	ParseArguments(filePtr);
}

::AbstractExpression* CInternalFuncCallAction::ToExpression(::SymbolTable* symTable) const
{
	auto proto = m_script->GetFnByBBId(m_eventIndex).prototype;
	auto protoName = proto->GetName();
	std::vector<::AbstractExpression*> args;
	for (auto arg : m_arguments)
	{
		args.push_back(arg->ToExpression(symTable));
	}
	return new AssignExpression(new VariableExpression(symTable->GetByAddress(0, EVariableType::Number, true)), new FunctionCallExpression(protoName, args));
}

void CInternalFuncCallAction::print(std::ostream& os) const
{
	auto proto = m_script->GetFnByBBId(m_eventIndex).prototype;
	auto protoName = proto->GetName();
	os << protoName;
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