#pragma once

#include <cstdint>
#include "StreamPtr.h"
#include "Statement.h"

class CIScript;

class CAction
{
protected:
	uint16_t m_actionId;
	uint16_t m_numOperands;
	CIScript* m_script;
	virtual void print(std::ostream& os) const = 0;
public:
	CAction(CIScript* script);
	virtual ~CAction() = default;
	virtual void Parse(StreamPtr& filePtr);
	virtual std::shared_ptr<CStatement> ToStatement() const = 0;

	friend std::ostream& operator<<(std::ostream& out, const CAction& o);
	static CAction* FindFactory(size_t id, CIScript* script, StreamPtr& fileptr);
};