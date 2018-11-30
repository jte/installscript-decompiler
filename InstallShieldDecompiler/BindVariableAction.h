#pragma once

#include "ActionWithArgs.h"

class CBindVariableAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	std::shared_ptr<CStatement> ToStatement() const;
public:
	CBindVariableAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};