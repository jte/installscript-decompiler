#pragma once

#include <vector>
#include <string>
#include <set>
#include <stdexcept>
#include <iostream>
#include <deque>
#include <map>
#include "Parser/Expressions.h"
#include "CFG/ControlFlowGraph.h"
#include "CFG/BasicBlock.h"
#include "GraphVizPrinter.h"

class IRGenerator : public AbstractVisitor
{
public:
	IRGenerator()
	{
		m_cfg = new ControlFlowGraph;

		BasicBlock* bb = CreateBB("entry");

		m_currentBB.push_back(bb);
		m_entryBB = bb;
	}
	int GenerateIR(AbstractExpression* exp);
	BasicBlock* CreateBB(std::string label);
	void CreateConditionalBr(IfExpression* ifExp, BasicBlock* thenBB, BasicBlock* elseBB);
	void CreateBr(GotoExpression* gotoExp, BasicBlock* targetBB);
	GraphVizPrinter GetGraphVizPrinter() const {
		return GraphVizPrinter(m_cfg);
	}
	void Visit(NumberExpression* exp) override;
	void Visit(StringExpression* exp) override;
	void Visit(VariableExpression* exp) override;
	void Visit(AssignExpression* exp) override;
	void Visit(IfExpression* exp) override;
	void Visit(ForExpression* exp) override;
	void Visit(BinaryExpression* exp) override;
	void Visit(ReturnExpression* exp) override;
	void Visit(GotoExpression* exp) override;
	void Visit(UnaryExpression* exp) override;
	void Visit(AddressOfExpression* exp) override;
	void Visit(ArrayAccessExpression* exp) override;
	void Visit(SetRefExpression* exp) override;
	void Visit(FunctionCallExpression* exp) override;
	void Visit(NopExpression* exp) override;
	void Visit(AbortExpression* exp) override;
	void Visit(ExitExpression* exp) override;
	void Visit(PropPutExpression* exp) override;
	void Visit(PropGetExpression* exp) override;
	void Visit(StringConstantExpression* exp) override;

	// not implemented, because visits only expressions
	virtual void Visit(BranchStatement* stmt) override {};
	virtual void Visit(AssignStatement* stmt) override {};
	virtual void Visit(ReturnStatement* stmt) override {};
	virtual void Visit(GotoStatement* stmt) override {};
	virtual void Visit(ExitStatement* stmt) override {};
	virtual void Visit(AbortStatement* stmt) override {};
	void LogError(const char*);

	ControlFlowGraph* m_cfg;
protected:
private:
	std::map<AbstractExpression*, bool> m_visited;
	BasicBlock* m_entryBB;
	std::deque<BasicBlock*> m_currentBB;
	std::deque<AbstractExpression*> m_stopExp;
};