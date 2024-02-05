#pragma once


#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CPropPutRefAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;

	AbstractExpression* ToExpression() const override {
		return new AssignExpression(new SetRefExpression(dynamic_cast<VariableExpression*>(m_arguments[0]->ToExpression())), m_arguments[1]->ToExpression());
	}
public:
	CPropPutRefAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};