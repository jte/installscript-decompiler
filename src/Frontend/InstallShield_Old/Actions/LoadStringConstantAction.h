#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

class CLoadStringConstantAction : public CActionWithArgs
{
public:
	CLoadStringConstantAction(CIScript* script, StreamPtr& filePtr);
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		return new AssignExpression(m_arguments[2]->ToExpression(symTable), new StringConstantExpression(m_arguments[1]->ToExpression(symTable)));
	}
private:
};

};
