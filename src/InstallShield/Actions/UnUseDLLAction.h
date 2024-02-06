#pragma once

#include "ActionWithArgs.h"

class CUnUseDLLAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
public:
	CUnUseDLLAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};