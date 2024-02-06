//
//  IRStatements.hpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef IRInstructions_hpp
#define IRInstructions_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <cstdint>
#include "AbstractVisitor.h"

class AbstractExpression;
class NumberExpression;
class VariableExpression;
class AssignExpression;
class IfExpression;
class ForExpression;
class BinaryExpression;
class ControlFlowGraph;
class BasicBlock;

enum class StatementType : uint8_t {
    ASSIGN = 0,
    BRANCH = 1,
    RETURN,
    GOTO
};

class AbstractStatement {
public:
    std::string displayLabel = "";
    StatementType type;
    virtual std::string Dump() = 0;
    AbstractStatement(StatementType type, std::string displayLabel) : type(type), displayLabel(displayLabel) {};
    virtual void Accept(AbstractVisitor* visitor) = 0;
};


class AssignStatement : public AbstractStatement {
public:
    AbstractExpression* var;
    AbstractExpression* rhs;

    std::string Dump() override;
    AssignStatement(AbstractExpression* var, AbstractExpression* rhs, std::string displayLabel) : AbstractStatement(StatementType::ASSIGN, displayLabel), var(var), rhs(rhs) {};
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class ReturnStatement : public AbstractStatement {
public:
    VariableExpression* var;

    std::string Dump() override;
    ReturnStatement(VariableExpression* var, std::string displayLabel) : AbstractStatement(StatementType::RETURN, displayLabel), var(var) {};
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class GotoStatement : public AbstractStatement {
public:
    AbstractExpression* targetExp;
    std::string targetLabel;
    std::string Dump() override;
    GotoStatement(AbstractExpression* targetExp, std::string displayLabel, std::string targetLabel) : AbstractStatement(StatementType::GOTO, displayLabel), targetExp(targetExp), targetLabel(targetLabel) {};
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class BranchStatement : public AbstractStatement {

private:
    BranchStatement(AbstractExpression* condition, BasicBlock* firstBranchBB, BasicBlock* secondBranchBB, bool isCond, std::string displayLabel) : AbstractStatement(StatementType::BRANCH, displayLabel), condition(condition), firstBranchBB(firstBranchBB), secondBranchBB(secondBranchBB), isConditional(isCond) {};
public:
    bool isConditional;
    AbstractExpression* condition;
    BasicBlock* firstBranchBB;
    BasicBlock* secondBranchBB;
    std::string Dump() override;
    BranchStatement(AbstractExpression* condition, BasicBlock* trueBranch, BasicBlock* falseBranch) : BranchStatement(condition, trueBranch, falseBranch, true, "") {};
    BranchStatement(BasicBlock* bb) : BranchStatement(nullptr, bb, nullptr, false, "") {};
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

#endif /* IRInstructions_hpp */