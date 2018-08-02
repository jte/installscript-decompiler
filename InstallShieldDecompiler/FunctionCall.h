#pragma once

#include "Expression.h"
#include <vector>

class CVariable;
class CPrototype;

class CFunctionCallExpression : public CExpression
{
private:
	std::vector<CExpression*> m_args;
	const CPrototype* m_proto;
public:
	CFunctionCallExpression(const CPrototype* proto, std::vector<CExpression*> args) :
		m_proto(proto), m_args(args)
	{}
	~CFunctionCallExpression()
	{}
	void print(std::ostream& out);
};