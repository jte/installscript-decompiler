#include "InternalFuncCallAction.h"
#include "CStrConst.h"
#include "CNumConst.h"
#include "IScript.h"
#include "InternalPrototype.h"
#include "vararg.h"
#include <cassert>
#include "FunctionCall.h"

CInternalFuncCallAction::CInternalFuncCallAction(CIScript* script, StreamPtr& filePtr) :
	CFuncCallAction(script, filePtr)
{
}

void CInternalFuncCallAction::print(std::ostream& os) const
{
	const CInternalPrototype* p = dynamic_cast<const CInternalPrototype*>(m_script->GetFnById(m_functionId).prototype);
	assert(p);
	auto name = p->GetName();
	auto flags = p->GetFlags();
	if (((uint32_t)flags & (uint32_t)CPrototype::Flags::IsExported) || !name.empty())
		os << "name:" << name;
	else
		os << "addr:" << p->GetBBId();
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

std::shared_ptr<CStatement> CInternalFuncCallAction::ToStatement() const
{
	// transform to LAST_RESULT = FuncCall(args)
	std::vector<CExpression*> exprs;
	CVariable* lastResult = new CVariable(ArgType::VariantArg, 0, false); // implicit
	exprs.push_back(lastResult);
	CFunctionCallExpression* fcall = new CFunctionCallExpression(m_script->GetFnByBBId(m_functionId).prototype, CVariable::FromScript(m_arguments));
	exprs.push_back(fcall);
	return std::make_shared<CStatement>(StatementType::Assign, exprs);
}