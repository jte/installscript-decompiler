#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CPropGetAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		AbstractExpression* var = nullptr;
		std::vector<AbstractExpression*> accessors;
		for (size_t i = 0; i < m_arguments.size(); i++)
		{
			if (i == 0)
			{
				var = m_arguments[i]->ToExpression();
			}
			else
			{
				auto expr = m_arguments[i]->ToExpression();
				if (dynamic_cast<StringExpression*>(expr))
				{
					auto str = dynamic_cast<StringExpression*>(expr);
					if (str->value.empty())
					{
						continue;
					}
				}
				accessors.push_back(expr);
			}
		}
		return new AssignExpression(new VariableExpression("GblVarObj0"), new PropGetExpression(var, accessors));
	}
public:
	CPropGetAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};