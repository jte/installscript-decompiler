#pragma once

#include "FuncCallAction.h"
#include "Parser/Expressions.h"

class CInternalFuncCallAction : public CFuncCallAction
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override;
public:
	CInternalFuncCallAction(CIScript* script, StreamPtr& filePtr);
	IArgument* ParseArgument(StreamPtr& filePtr) override;
};