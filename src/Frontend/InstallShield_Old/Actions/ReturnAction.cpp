#include "ReturnAction.h"

namespace oldis
{

	CReturnAction::CReturnAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{
		m_numOperands = 1;
		m_arguments.resize(m_numOperands);
		ParseArguments(filePtr);
	}

	void CReturnAction::print(std::ostream& os) const
	{
		os << "RETURN " << *(m_arguments[0]);
	}

};
