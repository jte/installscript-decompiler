#pragma once

#include "ActionWithArgs.h"

class CSetByteAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	CStatement ToStatement() const;
public:
	CSetByteAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};