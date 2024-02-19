#include "StrLengthCharsAction.h"
#include <cassert>
#include <string>

namespace newis
{

void CStrLengthCharsAction::print(std::ostream& os) const
{
}

CStrLengthCharsAction::CStrLengthCharsAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}

};
