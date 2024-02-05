#include "AbortAction.h"
#include <string>

CAbortAction::CAbortAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CAbortAction::print(std::ostream& os) const
{
	os << "abort";
}