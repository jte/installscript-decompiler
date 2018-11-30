#pragma once

#include "ActionWithArgs.h"

class CGotoAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	std::shared_ptr<CStatement> ToStatement() const;
public:
	CGotoAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};