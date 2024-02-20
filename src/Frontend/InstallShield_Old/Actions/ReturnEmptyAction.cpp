#include "ReturnEmptyAction.h"

namespace oldis
{

CReturnEmptyAction::CReturnEmptyAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CReturnEmptyAction::print(std::ostream& os) const
{
	os << "RETURN";
}

};
