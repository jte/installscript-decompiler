#pragma once

#include "ActionWithArgs.h"
#include "Statement.h"

class CReturnAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	CStatement ToStatement() const;
public:
	CReturnAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};