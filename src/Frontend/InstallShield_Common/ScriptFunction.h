#pragma once

#include "DataDeclList.h"
#include "ISBasicBlock.h"

class ScriptPrototype;

struct ScriptFunction
{
	ScriptFunction(ScriptPrototype* proto) :
		prototype(proto)
	{
	}
	ScriptFunction() :
		prototype(nullptr)
	{
	}
	CDataDeclList dataDeclList;
	ScriptPrototype* prototype;
	std::vector<size_t> labels;
	std::vector<ISBasicBlock> bbs;
};