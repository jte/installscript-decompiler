#include "StrFindAction.h"
#include <cassert>
#include <string>

namespace newis
{

void CStrFindAction::print(std::ostream& os) const
{
}

CStrFindAction::CStrFindAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}

};