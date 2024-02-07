#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CResizeAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		std::vector<AbstractExpression*> args;
		args.push_back(m_arguments[0]->ToExpression());
		args.push_back(m_arguments[1]->ToExpression());
		return new AssignExpression(new VariableExpression("GblVarObj0"), new FunctionCallExpression("Resize", args));
	}
public:
	CResizeAction(CIScript* script, StreamPtr& filePtr);
};