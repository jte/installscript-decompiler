#include "ActionWithArgs.h"
#include "NumConst.h"
#include "StrConst.h"
#include "vararg.h"

namespace oldis
{

CActionWithArgs::CActionWithArgs(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

IArgument* CActionWithArgs::ParseArgument(StreamPtr& filePtr)
{
	char argId = 0;

	filePtr.Read(argId);

	switch (argId)
	{
	case 0x41:
		return new CNumConst(filePtr);
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