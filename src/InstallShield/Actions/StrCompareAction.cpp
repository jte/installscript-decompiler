#include "StrCompareAction.h"
#include <cassert>
#include <string>

void CStrCompareAction::print(std::ostream& os) const
{
}

CStrCompareAction::CStrCompareAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}