#include "EndFuncAction.h"
#include <string>
#include <iostream>
#include "IScript.h"

CEndFuncAction::CEndFuncAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
	uint16_t addr = 0; // DataDeclList->numVariables (numLocalVariables + numArguments)
	filePtr.Read(addr);
	std::cout << "num variables:" << addr << std::endl;
	script->ReadVariantTable(filePtr);
	script->ReadSymFlagTable(filePtr);
}

void CEndFuncAction::print(std::ostream& os) const
{
	os << "EndFuncAction";
}