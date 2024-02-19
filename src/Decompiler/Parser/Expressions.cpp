#include "Expressions.h"
#include "Variables/Variable.h"

std::string VariableExpression::stringValue()
{
	return var ? var->GetName() : "<null>";
}