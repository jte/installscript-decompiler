#pragma once

#include "Statement.h"
#include <vector>
#include <cstdint>

class CBasicBlock
{
public:
	CBasicBlock(uint32_t address) :
		m_address(address)
	{}
	uint32_t GetAddress() const
	{
		return m_address;
	}
	void AddStatement(CStatement expr)
	{
		expr.SetOwner(this);
		m_statements.push_back(expr);
	}
	std::vector<CStatement> GetStatements() const
	{
		return m_statements;
	}
	friend std::ostream& operator<<(std::ostream& out, const CBasicBlock& o);
private:
	std::vector<CStatement> m_statements;
	uint32_t m_address;
};