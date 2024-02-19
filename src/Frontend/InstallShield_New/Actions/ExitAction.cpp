#include "ExitAction.h"
#include <string>

namespace newis
{

CExitAction::CExitAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CExitAction::print(std::ostream& os) const
{
	os << "EXIT";
}

};