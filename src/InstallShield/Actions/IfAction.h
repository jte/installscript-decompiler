#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CIfAction : public CActionWithArgs
{
public:
	CIfAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr), elseBranchTarget(0)
	{}
	uint32_t GetElseBranchLabelOffset() const;
	size_t elseBranchTarget;
protected:
	void print(std::ostream& os) const;
	AbstractExpression* ToExpression() const override
	{
		return m_arguments[1]->ToExpression();
	}
};