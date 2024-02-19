#pragma once

#include "Action.h"
#include <vector>
#include "IArgument.h"

namespace newis
{

class CActionWithArgs : public CAction
{
protected:
	std::vector<IArgument*> m_arguments;
public:
	CActionWithArgs(CIScript* script) : CAction(script) {}
	CActionWithArgs(CIScript* script, StreamPtr& filePtr);
	virtual void Parse(StreamPtr& filePtr) override;
	virtual IArgument* ParseArgument(StreamPtr& filePtr);
	virtual void ParseArguments(StreamPtr& filePtr);
};

};