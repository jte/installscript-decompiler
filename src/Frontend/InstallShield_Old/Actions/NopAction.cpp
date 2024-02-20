#include "NopAction.h"
#include <string>

namespace oldis
{

CNopAction::CNopAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CNopAction::print(std::ostream& os) const
{
	os << "NOP";
}

};
