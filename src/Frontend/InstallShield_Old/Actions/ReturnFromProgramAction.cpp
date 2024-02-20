#include "ReturnFromProgramAction.h"

namespace oldis
{

CReturnFromProgramAction::CReturnFromProgramAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CReturnFromProgramAction::print(std::ostream& os) const
{
	os << "RETURN";
}

};
