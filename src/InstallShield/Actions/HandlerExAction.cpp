#include "HandlerExAction.h"
#include <cassert>
#include <string>

void CHandlerExAction::print(std::ostream& os) const
{
}

CHandlerExAction::CHandlerExAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}