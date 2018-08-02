#include "Statement.h"
#include "Expression.h"
#include "FunctionCall.h"
#include "Literal.h"
#include "BasicBlock.h"

CStatement::CStatement(StatementType type) :  // actions without args
	m_type(type)
{}

CStatement::CStatement(StatementType type, std::vector<CExpression*> exprs) :
	m_type(type), m_exprs(exprs)
{

}

std::ostream& operator<<(std::ostream& out, const CStatement& o)
{
	switch (o.m_type)
	{
	case StatementType::Return:
		if (o.m_exprs.size() == 1)
			out << "return " << o.m_exprs[0] << ";";
		else
			out << "return;";
		break;
	case StatementType::NOP:
	case StatementType::FuncProlog:
	case StatementType::FuncEpilog:
		break;
	case StatementType::If:
		out << "If " << o.m_exprs[0] << " == "<<o.m_exprs[1] << " then" << std::endl;
		break;
	case StatementType::Goto:
	{
		CNumberLiteral* offset = dynamic_cast<CNumberLiteral*>(o.m_exprs[0]);
		int32_t offsetVal = offset->GetValue();
		uint32_t labelNum = o.m_owner->GetAddress() + offsetVal;
		out << "goto label_" << labelNum << ';' << std::endl;
	}
		break;
	case StatementType::Assign:
		out << o.m_exprs[0] << " = " << o.m_exprs[1] << std::endl;
		break;
	case StatementType::PropPut:
	{
		CStringLiteral* str = dynamic_cast<CStringLiteral*>(o.m_exprs[1]);

		out << o.m_exprs[0] << '.' << str->GetValue() << " = " << o.m_exprs[2] << std::endl;
	}
		break;
	case StatementType::PropGet:
		out << "PropGet: " << o.m_exprs[0] << '|' << o.m_exprs[1] << '|' << o.m_exprs[2] << std::endl;
		break;
	default:
		out << (int)o.m_type << std::endl;
		break;
	}
	
	return out;
}