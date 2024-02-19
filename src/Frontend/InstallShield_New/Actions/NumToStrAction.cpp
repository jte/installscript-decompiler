#include "NumToStrAction.h"
#include <cassert>
#include <string>

namespace newis
{

void CNumToStrAction::print(std::ostream& os) const
{
}

CNumToStrAction::CNumToStrAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}

};
