#include "EndFuncAction.h"
#include <string>
#include <iostream>
#include "IScript.h"

CEndFuncAction::CEndFuncAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
	CDataDeclList declList;
	declList.Parse(filePtr);
}

void CEndFuncAction::print(std::ostream& os) const
{
	os << "EndFuncAction";
}