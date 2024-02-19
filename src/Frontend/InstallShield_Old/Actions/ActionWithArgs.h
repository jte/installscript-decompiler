#pragma once

#include "Action.h"
#include <vector>
#include "IArgument.h"

namespace oldis
{

class CActionWithArgs : public CAction
{
protected:
	std::vector<IArgument*> m_arguments;
	uint16_t m_numOperands;
public:
	CActionWithArgs(CIScript* script) : CAction(script) {}
	CActionWithArgs(CIScript* script, StreamPtr& filePtr);
	virtual IArgument* ParseArgument(StreamPtr& filePtr);
	virtual void ParseArguments(StreamPtr& filePtr);
};

};