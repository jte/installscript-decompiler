#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CGotoAction : public CActionWithArgs
{
public:
	CGotoAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr), target(0)
	{}
	size_t target;
	int32_t GetLabelOffset() const;
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override 
	{
		return new GotoExpression(nullptr);
	}
};