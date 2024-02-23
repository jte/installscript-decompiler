#include "IRGenerator.h"
#include "IRStatements.h"
#include <algorithm>

void IRGenerator::CreateConditionalBr(IfExpression* ifExp, BasicBlock* thenBB, BasicBlock* elseBB)
{
	BasicBlock::AddLink(m_currentBB.back(), thenBB);
	BasicBlock::AddLink(m_currentBB.back(), elseBB);
	//BasicBlock::AddLink(thenBB, elseBB);
	BranchStatement* branchStmt = new BranchStatement(ifExp->conditionExp, thenBB, elseBB, ifExp->displayLabel);
	m_currentBB.back()->AddStatement(branchStmt);
}

void IRGenerator::CreateBr(GotoExpression* gotoExp, BasicBlock* targetBB)
{
	BasicBlock::AddLink(m_currentBB.back(), targetBB);
	BranchStatement* branchStmt = new BranchStatement(targetBB, gotoExp->displayLabel);
	m_currentBB.back()->AddStatement(branchStmt);
}

void IRGenerator::Visit(NumberExpression* exp) {
}

void IRGenerator::Visit(StringExpression* exp) {
}

void IRGenerator::Visit(VariableExpression* exp) {
}

void IRGenerator::Visit(PropPutExpression* exp) {

}

void IRGenerator::Visit(PropGetExpression* exp) {

}

void IRGenerator::Visit(StringConstantExpression* exp)
{

}

void IRGenerator::Visit(AssignExpression* exp) {
	m_currentBB.back()->AddStatement(new AssignStatement(exp->lhs, exp->rhs, exp->displayLabel));
}

void IRGenerator::Visit(ReturnExpression* exp)
{
	m_currentBB.back()->AddStatement(new ReturnStatement(exp->varExp, exp->displayLabel));
}

void IRGenerator::Visit(GotoExpression* exp)
{
	m_currentBB.back()->AddStatement(new GotoStatement(exp, exp->displayLabel, exp->targetLabel));
}

void IRGenerator::Visit(ExitExpression* exp)
{
	m_currentBB.back()->AddStatement(new ExitStatement(exp->displayLabel));
}

void IRGenerator::Visit(AbortExpression* exp)
{
	m_currentBB.back()->AddStatement(new AbortStatement(exp->displayLabel));
}

int IRGenerator::GenerateIR(AbstractExpression* exp)
{
	//std::cout << "GenerateIR: (BB_" << m_currentBB.top()->GetIndex() << ") " << exp->stringValue();
	if (std::find(m_stopExp.begin(), m_stopExp.end(), exp) != m_stopExp.end())
	{
		return 2;
	}
	if (m_visited[exp]) {
		//std::cout << " -- VISITED" << std::endl;
		return 0;
	}
	//std::cout << std::endl;
	exp->Accept(this);
	m_visited[exp] = true;
	return 1;
}

void IRGenerator::Visit(IfExpression* exp) {
	BasicBlock* thenBB = CreateBB(exp->stringValue() + " if_then");
	BasicBlock* contBB = CreateBB(exp->stringValue() + " if_cont");

	CreateConditionalBr(exp, thenBB, contBB);

	m_currentBB.push_back(thenBB);
	AbstractExpression* stopExp = exp->elseExp;
	m_stopExp.push_back(exp->elseExp);
	AbstractExpression* thenExp = exp->thenExp;
	while (thenExp) {
		if (thenExp == stopExp)
		{
			m_stopExp.pop_back();
			break;
		}
		if (GenerateIR(thenExp) == 2)
		{
			break;
		}
		thenExp = thenExp->next;
	}
	BasicBlock::AddLink(m_currentBB.back(), contBB);
	m_currentBB.pop_back();
	m_currentBB.push_back(contBB);
	AbstractExpression* next = stopExp;
	while (next)
	{
		if (GenerateIR(next) == 2)
		{
			break;
		}
		next = next->next;
	}
}

void IRGenerator::Visit(ForExpression* exp) {
}

void IRGenerator::Visit(BinaryExpression* exp)
{
}

void IRGenerator::Visit(UnaryExpression* exp)
{
}

void IRGenerator::Visit(AddressOfExpression* exp)
{
}

void IRGenerator::Visit(ArrayAccessExpression* exp)
{
}

void IRGenerator::Visit(SetRefExpression* exp)
{

}

void IRGenerator::Visit(FunctionCallExpression* exp)
{

}

void IRGenerator::Visit(NopExpression* exp)
{

}

void IRGenerator::LogError(const char* str) {
	fprintf(stderr, "Error: %s\n", str);
}

BasicBlock* IRGenerator::CreateBB(std::string label) {
	int nextIndex = (int)m_cfg->basicBlocks.size();
	BasicBlock* bb = new BasicBlock(nextIndex, label);
	m_cfg->AddBasicBlock(bb);
	return bb;
}