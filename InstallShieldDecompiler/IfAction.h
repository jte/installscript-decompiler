#pragma once

#include "ActionWithArgs.h"

class CIfAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	CStatement ToStatement() const;
public:
	CIfAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};