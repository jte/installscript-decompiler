#pragma once

#include "ActionWithArgs.h"

class CEndTryAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
public:
	CEndTryAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};