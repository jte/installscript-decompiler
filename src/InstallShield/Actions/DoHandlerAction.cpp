#include "DoHandlerAction.h"
#include <cassert>
#include <string>

void CDoHandlerAction::print(std::ostream& os) const
{
}

CDoHandlerAction::CDoHandlerAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}