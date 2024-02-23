#include "DisableAction.h"
#include <string>

namespace oldis
{

CDisableAction::CDisableAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{
	m_numOperands = 1;
	m_arguments.resize(m_numOperands);
	ParseArguments(filePtr);
}

void CDisableAction::print(std::ostream& os) const
{
	os << "DisableAction [0]: " << *(m_arguments[0]);
}

};
