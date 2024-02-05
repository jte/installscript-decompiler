#pragma once

#include "ActionWithArgs.h"

class CFuncCallAction : public CActionWithArgs
{
protected:
	uint16_t m_functionId;
public:
	CFuncCallAction(CIScript* script, StreamPtr& filePtr);
	virtual void Parse(StreamPtr& filePtr);
};