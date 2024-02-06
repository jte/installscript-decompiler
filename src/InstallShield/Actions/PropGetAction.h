#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CPropGetAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	//AbstractExpression* ToExpression() const override {
	//	return new AssignExpression(new VariableExpression("GblObj0"), m_arguments[1]->ToExpression());
	//}
	//1c1: Get v{ CVariantArg } - 101 "" 0

public:
	CPropGetAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};