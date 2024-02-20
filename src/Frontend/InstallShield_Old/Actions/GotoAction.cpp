#include "GotoAction.h"

namespace oldis
{

CGotoAction::CGotoAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{
	uint8_t type;//0x70
	
	filePtr.Read(type);
	filePtr.Read(m_bbIndex);
}

void CGotoAction::print(std::ostream& os) const
{
	os << "GOTO " << m_bbIndex;
}

};
