#include "ActionWithArgs.h"
#include "CNumConst.h"
#include "CStrConst.h"
#include "vararg.h"

CActionWithArgs::CActionWithArgs(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CActionWithArgs::Parse(StreamPtr& filePtr)
{
	CAction::Parse(filePtr);
	m_arguments.resize(m_numOperands);
	ParseArguments(filePtr);
}
/**
.rdata:100D50F4     apArgs          dd offset vararg_CStrArg__constructor
.rdata : 100D50F8                     dd offset vararg_CNumArg__constructor
.rdata : 100D50FC                     dd offset CStrConst__new
.rdata : 100D5100                     dd offset CNumConst__new
.rdata : 100D5104                     dd offset vararg_CVariantArg__constructor*/

IArgument* CActionWithArgs::ParseArgument(StreamPtr& filePtr)
{
	char argId = 0;

	filePtr.Read(argId);

	switch (argId - 4)
	{
	case 0:
		return new vararg<ArgType::StrArg, false>(filePtr);
		break;
	case 1:
		return new vararg<ArgType::NumArg, false>(filePtr);
		break;
	case 2:
		return new CStrConst(filePtr);
		break;
	case 3:
		return new CNumConst(filePtr);
		break;
	case 4:
		return new vararg<ArgType::VariantArg, false>(filePtr);
		break;
	default:
		return nullptr;
	}

	//return apArgs[(argId - 4) % _countof(apArgs)](filePtr);
}

void CActionWithArgs::ParseArguments(StreamPtr& filePtr)
{
	for (auto& it : m_arguments)
	{
		it = ParseArgument(filePtr);
	}
}