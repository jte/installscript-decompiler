#include "DoHandlerAction.h"
#include <cassert>
#include <string>

namespace newis
{

void CDoHandlerAction::print(std::ostream& os) const
{
}

CDoHandlerAction::CDoHandlerAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}

};