#include "ActionWithArgs.h"
#include "NumConst.h"
#include "StrConst.h"
#include "vararg.h"

namespace oldis
{

CActionWithArgs::CActionWithArgs(CIScript* script, StreamPtr& filePtr) :
	CAction(script), m_numOperands(0)
{
	Parse(filePtr);
}

IArgument* CActionWithArgs::ParseArgument(StreamPtr& filePtr)
{
	char argId = 0;

	filePtr.Read(argId);
	
	uint8_t argType = argId & 0xf0;
	uint8_t argFlags = argId & 0x0f;

	switch (argType)
	{
	case 0x10:
	{
		char unknown;
		filePtr.Read(unknown);
		break;
	}
	case 0x30: 
	{
		return new vararg<ArgType::VariantArg, false>(filePtr);
		break;
	}
	case 0x40: 
	{
		if (argFlags & 1)
		{
			return new CNumConst(filePtr);
		}
		else
		{
			return new vararg<ArgType::NumArg, false>(filePtr);
		}
		break;
	}
	case 0x50:
	{
		return new vararg<ArgType::VariantArg, false>(filePtr);
		break;
	}
	case 0x60:
	{
		if (argFlags & 1)
		{
			return new CStrConst(filePtr);
		}
		else
		{
			return new vararg<ArgType::StrArg, false>(filePtr);
		}
		break;
	}
	// TODO:check what's with 0x70, 0x80, 0x90
	default:
		break;
	}
	throw std::runtime_error("Invalid argument type " + std::to_string(argId));
}

void CActionWithArgs::ParseArguments(StreamPtr& filePtr)
{
	for (auto& it : m_arguments)
	{
		it = ParseArgument(filePtr);
	}
}

};