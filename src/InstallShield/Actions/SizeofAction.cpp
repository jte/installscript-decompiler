#include "SizeofAction.h"
#include <cassert>
#include <string>

void CSizeofAction::print(std::ostream& os) const
{
}

CSizeofAction::CSizeofAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}