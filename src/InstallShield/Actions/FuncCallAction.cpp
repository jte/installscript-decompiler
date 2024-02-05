#include "FuncCallAction.h"

CFuncCallAction::CFuncCallAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script)
{
	Parse(filePtr);
}

void CFuncCallAction::Parse(StreamPtr& filePtr)
{
	filePtr.Read(m_actionId);
	filePtr.Read(m_functionId);
	uint16_t numArguments = 0;
	filePtr.Read(numArguments);
	m_arguments.resize(numArguments);
	ParseArguments(filePtr);
}