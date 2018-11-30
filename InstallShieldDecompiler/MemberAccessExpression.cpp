#include "MemberAccessExpression.h"
#include "Variable.h"

void CMemberAccessExpression::print(std::ostream& out)
{
	out << m_base->GetName() << '.';
	if (!m_byString.empty())
	{
		out << m_byString;
	}
	else
		out << m_byIndex;
}