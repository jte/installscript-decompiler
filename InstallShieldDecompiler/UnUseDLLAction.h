#pragma once

#include "ActionWithArgs.h"

class CUnUseDLLAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	std::shared_ptr<CStatement> ToStatement() const;
public:
	CUnUseDLLAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};