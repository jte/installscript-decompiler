#pragma once

#include "Statement.h"
#include <vector>
#include <cstdint>

class CBasicBlock
{
public:
	CBasicBlock()
	{}
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
};