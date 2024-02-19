#pragma once

#include <cstdint>
#include <memory>
#include "StreamPtr.h"
#include "Variables/SymbolTable.h"

class CIScript;
class AbstractExpression;
class CFunction;
class SymbolTable;

class CAction
{
protected:
	uint16_t m_actionId;
	uint16_t m_numOperands;
	CIScript* m_script;
	size_t m_bbId;
	virtual void print(std::ostream& os) const = 0;
public:
	CAction(CIScript* script);
	void SetBBId(size_t bbId) 
	{
		m_bbId = bbId;
	}
	size_t GetBBId() 
	{
		return m_bbId;
	}
	virtual ~CAction() = default;
	virtual void Parse(StreamPtr& filePtr);
	virtual AbstractExpression* ToExpression(SymbolTable* symTable) const { return nullptr; }

	friend std::ostream& operator<<(std::ostream& out, const CAction& o);
	static CAction* FindFactory(size_t id, CIScript* script, StreamPtr& fileptr);
};