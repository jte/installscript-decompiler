#include "AbortAction.h"
#include <string>

namespace newis
{

CAbortAction::CAbortAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CAbortAction::print(std::ostream& os) const
{
	os << "abort";
}

};
