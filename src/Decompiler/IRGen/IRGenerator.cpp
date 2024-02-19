#include "IRGenerator.h"
#include "IRStatements.h"

void IRGenerator::CreateConditionalBr(IfExpression* ifExp, BasicBlock* thenBB, BasicBlock* elseBB)
{
	BasicBlock::AddLink(m_currentBB.top(), thenBB);
	BasicBlock::AddLink(m_currentBB.top(), elseBB);
	BasicBlock::AddLink(thenBB, elseBB);
	BranchStatement* branchStmt = new BranchStatement(ifExp->conditionExp, thenBB, elseBB, ifExp->displayLabel);
	m_currentBB.top()->AddStatement(branchStmt);
}

void IRGenerator::CreateBr(GotoExpression* gotoExp, BasicBlock* targetBB)
{
	BasicBlock::AddLink(m_currentBB.top(), targetBB);
	BranchStatement* branchStmt = new BranchStatement(targetBB, gotoExp->displayLabel);
	m_currentBB.top()->AddStatement(branchStmt);
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

void IRGenerator::Visit(AssignExpression* exp) {
	m_currentBB.top()->AddStatement(new AssignStatement(exp->lhs, exp->rhs, exp->displayLabel));
}

void IRGenerator::Visit(ReturnExpression* exp)
{
	m_currentBB.top()->AddStatement(new ReturnStatement(exp->varExp, exp->displayLabel));
}

void IRGenerator::Visit(GotoExpression* exp)
{
	m_currentBB.top()->AddStatement(new GotoStatement(exp, exp->displayLabel, exp->targetLabel));
}

void IRGenerator::Visit(ExitExpression* exp)
{
	m_currentBB.top()->AddStatement(new ExitStatement(exp->displayLabel));
}

void IRGenerator::Visit(AbortExpression* exp)
{
	m_currentBB.top()->AddStatement(new AbortStatement(exp->displayLabel));
}

void IRGenerator::Visit(IfExpression* exp) {
	BasicBlock* thenBB = CreateBB("then");
	BasicBlock* contBB = CreateBB("if_cont");

	CreateConditionalBr(exp, thenBB, contBB);

	m_currentBB.push(thenBB);
	m_stopExp.push(exp->elseExp);
	AbstractExpression* thenExp = exp->thenExp;
	while (thenExp) {
		if (thenExp == m_stopExp.top())
		{
			m_stopExp.pop();
			break;
		}
		GenerateIR(thenExp);
		thenExp = thenExp->next;
	}
	m_currentBB.pop();
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