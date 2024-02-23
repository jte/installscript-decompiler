#include "EnableAction.h"
#include <string>

namespace oldis
{

CEnableAction::CEnableAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{
	m_numOperands = 1;
	m_arguments.resize(m_numOperands);
	ParseArguments(filePtr);
}

void CEnableAction::print(std::ostream& os) const
{
	os << "EnableAction [0]: " << *(m_arguments[0]);
}

};
