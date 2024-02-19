#include "ActionWithArgs.h"
#include "NumConst.h"
#include "StrConst.h"
#include "vararg.h"

namespace newis
{

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
		throw std::runtime_error("Invalid argument type " + std::to_string(argId));
	}
}

void CActionWithArgs::ParseArguments(StreamPtr& filePtr)
{
	for (auto& it : m_arguments)
	{
		it = ParseArgument(filePtr);
	}
}

};