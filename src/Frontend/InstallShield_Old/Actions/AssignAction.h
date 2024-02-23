#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"

namespace oldis
{

class CAssignAction : public CActionWithArgs
{
public:
	CAssignAction(CIScript* script, StreamPtr& filePtr);
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override {
		return new AssignExpression(m_arguments[0]->ToExpression(symTable), m_arguments[1]->ToExpression(symTable));
	}
private:
};

};
