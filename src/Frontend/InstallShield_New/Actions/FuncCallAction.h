#pragma once

#include "ActionWithArgs.h"

namespace newis
{

class CFuncCallAction : public CActionWithArgs
{
protected:
	uint16_t m_functionId;
public:
	CFuncCallAction(CIScript* script, StreamPtr& filePtr);
	void Parse(StreamPtr& filePtr) override;
};

};