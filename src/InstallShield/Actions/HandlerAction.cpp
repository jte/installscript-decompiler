#include "HandlerAction.h"
#include <cassert>
#include <string>

void CHandlerAction::print(std::ostream& os) const
{
}

CHandlerAction::CHandlerAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}