#pragma once

#include "ActionWithArgs.h"

class CUseDLLAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
public:
	CUseDLLAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};