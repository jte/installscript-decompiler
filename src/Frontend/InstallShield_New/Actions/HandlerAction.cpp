#include "HandlerAction.h"
#include <cassert>
#include <string>

namespace newis
{

void CHandlerAction::print(std::ostream& os) const
{
}

CHandlerAction::CHandlerAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}

};
