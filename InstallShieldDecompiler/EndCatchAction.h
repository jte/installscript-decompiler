#pragma once

#include "ActionWithArgs.h"

class CEndCatchAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	CStatement ToStatement() const;
public:
	CEndCatchAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};