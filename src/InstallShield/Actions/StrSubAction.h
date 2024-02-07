#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CStrSubAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		std::vector<AbstractExpression*> args;
		args.push_back(m_arguments[0]->ToExpression());
		args.push_back(m_arguments[1]->ToExpression());
		args.push_back(m_arguments[2]->ToExpression());
		args.push_back(m_arguments[3]->ToExpression());
		return new AssignExpression(new VariableExpression("GblVarObj0"), new FunctionCallExpression("StrSub", args));
	}
public:
	CStrSubAction(CIScript* script, StreamPtr& filePtr);
};