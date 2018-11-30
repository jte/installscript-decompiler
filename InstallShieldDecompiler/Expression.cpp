#include "Expression.h"
#include "Variable.h"
#include "IArgument.h"
#include "vararg.h"

std::vector<CExpression*> CExpression::FromScript(std::vector<class IArgument*> args)
{
	std::vector<CExpression*> vargs;
	for (auto& ia : args)
	{
		vargs.push_back(ia->ToExpression());
	}
	return vargs;
}

std::ostream& operator<<(std::ostream& out, CExpression* e)
{
	e->print(out);
	return out;
}