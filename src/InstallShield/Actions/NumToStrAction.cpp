#include "NumToStrAction.h"
#include <cassert>
#include <string>

void CNumToStrAction::print(std::ostream& os) const
{
}

CNumToStrAction::CNumToStrAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}