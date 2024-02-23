#include "SetTitleAction.h"
#include <string>

namespace oldis
{

CSetTitleAction::CSetTitleAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{
	m_numOperands = 3;
	m_arguments.resize(m_numOperands);
	ParseArguments(filePtr);
}

void CSetTitleAction::print(std::ostream& os) const
{
	os << "SetTitleAction [0]: " << *(m_arguments[0]);
}

};
