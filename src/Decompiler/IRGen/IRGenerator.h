#pragma once

#include <vector>
#include <string>
#include <set>
#include <stdexcept>
#include <iostream>
#include <stack>
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

		m_currentBB.push(bb);
		m_entryBB = bb;
	}
	bool GenerateIR(AbstractExpression* exp) {
		size_t debug = 0;
		if (dynamic_cast<IfExpression*>(exp))
		{
			debug = 1;
		}
//		std::cout << "GenerateIR: (" << m_currentBB.top()->stringValue() << ") " << exp->stringValue();
		if (m_visited[exp]) {
			//std::cout << " -- VISITED" << std::endl;
			return false;
		}
		/**if (m_stopExp.size() && m_stopExp.top() == exp) {
			m_stopExp.pop();
			m_currentBB.pop();
			std::cout << " -- STOP" << std::endl;
			return false;
		}*/
		//std::cout << std::endl;
		exp->Accept(this);
		m_visited[exp] = true;
		return true;
	}
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

	// not implemented, because visits only expressions
	virtual void Visit(BranchStatement* stmt) override {};
	virtual void Visit(AssignStatement* stmt) override {};
	virtual void Visit(ReturnStatement* stmt) override {};
	virtual void Visit(GotoStatement* stmt) override {};
	void LogError(const char*);

	ControlFlowGraph* m_cfg;
protected:
private:
	std::map<AbstractExpression*, bool> m_visited;
	BasicBlock* m_entryBB;
	std::stack<BasicBlock*> m_currentBB;
	std::stack<AbstractExpression*> m_stopExp;
};