#include "StrSubAction.h"
#include <cassert>
#include <string>

void CStrSubAction::print(std::ostream& os) const
{
}

CStrSubAction::CStrSubAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}