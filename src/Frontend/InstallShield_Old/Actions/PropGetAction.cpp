#include "PropGetAction.h"
#include <string>

namespace oldis
{

CPropGetAction::CPropGetAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{
	m_numOperands = 5;
	m_arguments.resize(m_numOperands);
	ParseArguments(filePtr);
}

void CPropGetAction::print(std::ostream& os) const
{
	os << "PropGet [0]: " << *(m_arguments[0]) << " [1]: " << *(m_arguments[1]) << " [2]: " << *(m_arguments[2]) << " [3]: " << *(m_arguments[3]) << " [4]: " << *(m_arguments[4]);
}

};
