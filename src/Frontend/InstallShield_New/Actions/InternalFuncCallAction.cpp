#include "InternalFuncCallAction.h"
#include "StrConst.h"
#include "NumConst.h"
#include "IScript.h"
#include "Prototype/InternalPrototype.h"
#include "vararg.h"
#include <cassert>
#include "ScriptPrototype.h"

namespace newis
{

CInternalFuncCallAction::CInternalFuncCallAction(CIScript* script, StreamPtr& filePtr) :
	CFuncCallAction(script, filePtr)
{
}

void CInternalFuncCallAction::print(std::ostream& os) const
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

::AbstractExpression* CInternalFuncCallAction::ToExpression(::SymbolTable* symTable) const
{
	/**
	const CInternalPrototype* p = dynamic_cast<const CInternalPrototype*>(m_script->GetFnById(m_functionId).prototype);
	auto name = p->GetName();
	auto flags = p->GetFlags();
	std::stringstream ss;
	if (((uint32_t)flags & (uint32_t)CPrototype::Flags::IsExported) || !name.empty())
		ss << name;
	else
		ss << "Func_" << p->GetBBId();*/
	std::stringstream ss;
	ss << m_script->GetFnById(m_functionId).prototype->GetName();

	std::vector<::AbstractExpression*> args;
	for (const auto& arg : m_arguments)
	{
		args.push_back(arg->ToExpression(symTable));
	}

	return new AssignExpression(new VariableExpression(symTable->GetByName("LAST_RESULT", EVariableType::Variant, true)), new FunctionCallExpression(ss.str(), args));
}

IArgument* CInternalFuncCallAction::ParseArgument(StreamPtr& filePtr)
{
	char argId = 0;

	filePtr.Read(argId);

	switch (argId - 4)
	{
	case 0:
		return new vararg<ArgType::StrArg, true>(filePtr);
		break;
	case 1:
		return new vararg<ArgType::NumArg, true>(filePtr);
		break;
	case 2:
		return new CStrConst(filePtr);
		break;
	case 3:
		return new CNumConst(filePtr);
		break;
	case 4:
		return new vararg<ArgType::VariantArg, true>(filePtr);
		break;
	default:
		throw std::logic_error("Invalid argument type");
	}

	//return pParams[(argId - 4) % _countof(apArgs)](filePtr);
}

};