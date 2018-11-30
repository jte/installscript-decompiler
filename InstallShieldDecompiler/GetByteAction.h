#pragma once

#include "ActionWithArgs.h"

class CGetByteAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	std::shared_ptr<CStatement> ToStatement() const;
public:
	CGetByteAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};