#include "ArrayAccessExpression.h"
#include "Variable.h"

void CArrayAccessExpression::print(std::ostream& out)
{
	static char brackets[2][2] = { {'[',']'}, {'(',')'} };

	out << m_base->GetName() << brackets[(int)m_type][0];
	out << m_index << brackets[(int)m_type][1];
}