#pragma once

#include "FuncCallAction.h"

class CDLLFuncCallAction : public CFuncCallAction
{
protected:
	void print(std::ostream& os) const;
	std::shared_ptr<CStatement> ToStatement() const;
public:
	CDLLFuncCallAction(CIScript* script, StreamPtr& filePtr);
};