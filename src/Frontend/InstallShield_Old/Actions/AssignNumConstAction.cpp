#include "AssignNumConstAction.h"

namespace oldis
{

CAssignNumConstAction::CAssignNumConstAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script)
{
	Parse(filePtr);
	m_numOperands = 2;
	m_arguments.resize(m_numOperands);
	ParseArguments(filePtr);
}

void CAssignNumConstAction::print(std::ostream& os) const
{
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

};
