#include "AddressOfExpression.h"
#include "Variable.h"

void CAddressOfExpression::print(std::ostream& out)
{
	out << '&' << m_var->GetName();
}