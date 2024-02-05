#include "NOPAction.h"
#include <string>

CNOPAction::CNOPAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CNOPAction::print(std::ostream& os) const
{
	os << "NOP";
}