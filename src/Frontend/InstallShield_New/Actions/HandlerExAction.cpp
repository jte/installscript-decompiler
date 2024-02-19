#include "HandlerExAction.h"
#include <cassert>
#include <string>

namespace newis
{

void CHandlerExAction::print(std::ostream& os) const
{
}

CHandlerExAction::CHandlerExAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}

};
