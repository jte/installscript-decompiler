#pragma once

#include "FuncCallAction.h"
#include "Parser/Expressions.h"

class CDLLFuncCallAction : public CFuncCallAction
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override;
public:
	CDLLFuncCallAction(CIScript* script, StreamPtr& filePtr);
};