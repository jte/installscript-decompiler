#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CPropPutAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		AbstractExpression* var = nullptr;
		std::vector<AbstractExpression*> accessors;
		AbstractExpression* rhs = nullptr;
		for (size_t i = 0; i < m_arguments.size(); i++)
		{
			if (i == 0)
			{
				var = m_arguments[i]->ToExpression();
			}
			else if (i == m_arguments.size() - 1)
			{
				rhs = m_arguments[i]->ToExpression();
			}
			else
			{
				accessors.push_back(m_arguments[i]->ToExpression());
			}
		}
		return new AssignExpression(new PropPutExpression(var, accessors), rhs);
	}
public:
	CPropPutAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};