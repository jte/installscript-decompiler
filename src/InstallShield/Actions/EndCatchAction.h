#pragma once

#include "ActionWithArgs.h"

class CEndCatchAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
public:
	CEndCatchAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};