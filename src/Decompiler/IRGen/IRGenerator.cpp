#include "IRGenerator.h"
#include "IRStatements.h"

void IRGenerator::CreateConditionalBr(AbstractExpression* condition, BasicBlock* thenBB, BasicBlock* elseBB)
{
	BasicBlock::AddLink(m_currentBB.top(), thenBB);
	//BasicBlock::AddLink(m_currentBB.top(), elseBB);
	BranchStatement* branchStmt = new BranchStatement(condition, thenBB, elseBB);
	m_currentBB.top()->AddStatement(branchStmt);
}

void IRGenerator::CreateBr(BasicBlock* targetBB)
{
	BasicBlock::AddLink(m_currentBB.top(), targetBB);
	BranchStatement* branchStmt = new BranchStatement(targetBB);
	m_currentBB.top()->AddStatement(branchStmt);
}

void IRGenerator::Visit(NumberExpression* exp) {
}

void IRGenerator::Visit(StringExpression* exp) {
}

void IRGenerator::Visit(VariableExpression* exp) {
}

void IRGenerator::Visit(AssignExpression* exp) {
	m_currentBB.top()->AddStatement(new AssignStatement(exp->varExp, exp->expr, exp->displayLabel));
}

void IRGenerator::Visit(ReturnExpression* exp)
{
	m_currentBB.top()->AddStatement(new ReturnStatement(exp->varExp, exp->displayLabel));
}

void IRGenerator::Visit(GotoExpression* exp)
{
	m_currentBB.top()->AddStatement(new GotoStatement(exp, exp->displayLabel, exp->targetLabel));
}


void IRGenerator::Visit(IfExpression* exp) {
	BasicBlock* thenBB = CreateBB("then");

	CreateConditionalBr(exp->conditionExp, thenBB, nullptr);

	m_currentBB.push(thenBB);
	m_stopExp.push(exp->elseExp);
	AbstractExpression* thenExp = exp->thenExp;
	bool ok = true;
	while (thenExp && ok) {
		ok = GenerateIR(thenExp);
		thenExp = thenExp->next;
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