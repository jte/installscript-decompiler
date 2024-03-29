#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

namespace newis
{

class CIfAction : public CActionWithArgs
{
public:
	CIfAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr), elseBranchTarget(0)
	{}
	uint32_t GetElseBranchLabelOffset() const;
	size_t elseBranchTarget;
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		return new IfExpression(m_arguments[1]->ToExpression(symTable), nullptr, nullptr);
	}
};

};