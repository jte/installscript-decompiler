#pragma once

#include "FuncCallAction.h"

class CDLLFuncCallAction : public CFuncCallAction
{
protected:
	void print(std::ostream& os) const override;
public:
	CDLLFuncCallAction(CIScript* script, StreamPtr& filePtr);
};