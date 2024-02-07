#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CUseDLLAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		std::vector<AbstractExpression*> args;
		args.push_back(m_arguments[0]->ToExpression());
		return new AssignExpression(new VariableExpression("GblVarObj0"), new FunctionCallExpression("UseDLL", args));
	}
public:
	CUseDLLAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};