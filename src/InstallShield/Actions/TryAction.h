#pragma once

#include "ActionWithArgs.h"

class CTryAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
public:
	CTryAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};