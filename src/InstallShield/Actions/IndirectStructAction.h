#pragma once

#include "ActionWithArgs.h"
#include <cassert>
#include "Parser/Expressions.h"

class CIndirectStructAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override
	{
	}
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression;
	}
public:
	CIndirectStructAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script)
	{
		uint16_t actionId = 0;
		filePtr.Read(actionId);

		uint16_t structId = 0;
		filePtr.Read(structId);

		m_numOperands = 1;

		m_arguments.resize(1);
		ParseArguments(filePtr);
	}
};