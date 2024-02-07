#include "ResizeAction.h"
#include <cassert>
#include <string>

void CResizeAction::print(std::ostream& os) const
{
}

CResizeAction::CResizeAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}