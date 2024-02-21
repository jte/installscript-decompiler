#include "LoadStringConstantAction.h"

namespace oldis
{

CLoadStringConstantAction::CLoadStringConstantAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{
	m_numOperands = 3;
	m_arguments.resize(m_numOperands);
	ParseArguments(filePtr);
}

void CLoadStringConstantAction::print(std::ostream& os) const
{
	os << *(m_arguments[2]) << " = " << *(m_arguments[1]) << " (" << *(m_arguments[0]) << ")";
}

};
