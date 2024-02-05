#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CAddressOfAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	AbstractExpression* ToExpression() const override {
		return new AssignExpression(m_arguments[0]->ToExpression(), new AddressOfExpression(m_arguments[1]->ToExpression()));
	}
public:
	CAddressOfAction(CIScript* script, StreamPtr& filePtr);
};