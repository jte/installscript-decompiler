#pragma once

#include <cstdint>
#include <memory>
#include "StreamPtr.h"
#include "Variables/SymbolTable.h"
#include "Parser/Expressions.h"
#include "InstallShield_Common/Action.h"

namespace oldis
{

class CIScript;
class AbstractExpression;
class SymbolTable;

class CAction : public ::CAction
{
protected:
	uint16_t m_actionId;
	CIScript* m_script;
public:
	CAction(CIScript* script);
	virtual ~CAction() = default;
	virtual void Parse(StreamPtr& filePtr);
	virtual ::AbstractExpression* ToExpression(::SymbolTable* symTable) const { return nullptr; }

	static CAction* FindFactory(size_t id, CIScript* script, StreamPtr& fileptr);
};

};