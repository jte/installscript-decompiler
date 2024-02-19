#include "StrToNumAction.h"
#include <cassert>
#include <string>

namespace newis
{

void CStrToNumAction::print(std::ostream& os) const
{
}

CStrToNumAction::CStrToNumAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}

};