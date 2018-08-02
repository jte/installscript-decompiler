#pragma once

#include "ActionWithArgs.h"

class CAssignAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	CStatement ToStatement() const;
public:
	CAssignAction(CIScript* script, StreamPtr& filePtr);
};