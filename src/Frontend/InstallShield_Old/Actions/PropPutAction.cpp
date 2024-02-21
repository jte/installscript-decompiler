#include "PropPutAction.h"
#include <string>

namespace oldis
{

CPropPutAction::CPropPutAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{
	m_numOperands = 5;
	m_arguments.resize(m_numOperands);
	ParseArguments(filePtr);
}

void CPropPutAction::print(std::ostream& os) const
{
	os << "PropPut [0]: " << *(m_arguments[0]) << " [1]: " << *(m_arguments[1]) << " [2]: " << *(m_arguments[2]) << " [3]: " << *(m_arguments[3]) << " [4]: " << *(m_arguments[4]);
}

};
