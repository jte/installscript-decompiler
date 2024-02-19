#include "NOPAction.h"
#include <string>

namespace newis
{

CNOPAction::CNOPAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CNOPAction::print(std::ostream& os) const
{
	os << "NOP";
}

};
