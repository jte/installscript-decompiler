//
//  AbstractExpresssion.hpp
//  Compiler
//
//  Created by Филипп Федяков on 27.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef AbstractExpresssion_hpp
#define AbstractExpresssion_hpp

#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include "AbstractVisitor.h"

enum class BinaryExprType
{
    BinAdd,
    BinSub,
    BinMod,
    BinLT,
    BinGT,
    BinLTE,
    BinGTE,
    BinEq,
    BinNEq,
    BinMul,
    BinDiv,
    BitAnd,
    BitOr,
    BitXor,
    BitShl,
    BitShr,
    LogAnd,
    LogOr
};

class CVariable;

class AbstractExpression {
public:
    virtual std::string stringValue() { return ""; };
    AbstractExpression* next = nullptr;
    std::string displayLabel = "";
    static std::string GetNextLabel() {
        static int labelCounter = 1;
        return "label_" + std::to_string(labelCounter++);
    }
    virtual ~AbstractExpression() = default;
    virtual void Accept(AbstractVisitor* visitor) = 0;
};

class StringExpression : public AbstractExpression {
public:
    std::string value;
    std::string stringValue() override { return "\"" + value + "\""; }
    StringExpression(std::string value) : value(value) {};
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class NumberExpression : public AbstractExpression {
public:
    int value;
    std::string stringValue() override { return std::to_string(value); }
    NumberExpression(int value) : value(value) {};
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class VariableExpression : public AbstractExpression {
public:
    CVariable* var;
    std::string stringValue() override;

    VariableExpression(CVariable* var) : var(var) {}

    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class AssignExpression : public AbstractExpression {
public:
    std::string varName() { return lhs->stringValue(); };
    AbstractExpression* rhs;
    AbstractExpression* lhs;
    std::string stringValue() override { return lhs->stringValue() + " = " + (rhs ? rhs->stringValue() : "null"); }

    AssignExpression(AbstractExpression* lhs, AbstractExpression* rhs) : lhs(lhs), rhs(rhs) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class ReturnExpression : public AbstractExpression {
public:
    AbstractExpression* varExp;

    ReturnExpression(AbstractExpression* varExp) : varExp(varExp) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class AbortExpression : public AbstractExpression {
public:
    AbortExpression() {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class ExitExpression : public AbstractExpression {
public:
    ExitExpression() {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class PropPutExpression : public AbstractExpression {
public:
    AbstractExpression* var;
    std::vector<AbstractExpression*> accessors;

    std::string stringValue() override {
        std::stringstream val;
        val << var->stringValue();
        for (auto ac : accessors)
        {
            if (dynamic_cast<NumberExpression*>(ac) || dynamic_cast<VariableExpression*>(ac))
            {
                val << "(" << ac->stringValue() << ")";
            }
            else if (dynamic_cast<StringExpression*>(ac))
            {
                auto str = dynamic_cast<StringExpression*>(ac);
                val << "." << str->value;
            }
            else
            {
                throw std::runtime_error("Invalid accessor expression in prop put expression.");
            }
        }
        return val.str();
    }

    PropPutExpression(AbstractExpression* var, std::vector<AbstractExpression*> accessors) : var(var), accessors(accessors) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class PropGetExpression : public AbstractExpression {
public:
    AbstractExpression* var;
    std::vector<AbstractExpression*> accessors;

    std::string stringValue() override {
        std::stringstream val;
        val << var->stringValue();
        for (auto ac : accessors)
        {
            if (dynamic_cast<NumberExpression*>(ac) || dynamic_cast<VariableExpression*>(ac))
            {
                val << "(" << ac->stringValue() << ")";
            }
            else if (dynamic_cast<StringExpression*>(ac))
            {
                auto str = dynamic_cast<StringExpression*>(ac);
                val << "." << str->value;
            }
            else
            {
                throw std::runtime_error("Invalid accessor expression in prop get expression.");
            }
        }
        return val.str();
    }

    PropGetExpression(AbstractExpression* var, std::vector<AbstractExpression*> accessors) : var(var), accessors(accessors) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class GotoExpression : public AbstractExpression {
public:
    AbstractExpression* targetExp;
    std::string targetLabel;

    std::string stringValue() override { return "goto " + targetLabel; }

    GotoExpression(AbstractExpression* targetExp) : targetExp(targetExp) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class IfExpression : public AbstractExpression {
public:
    AbstractExpression* conditionExp, * thenExp, * elseExp;
    std::string stringValue() override { return "IF " + conditionExp->stringValue(); };
    IfExpression(AbstractExpression* conditionExp, AbstractExpression* thenExp, AbstractExpression* elseExp) : conditionExp(conditionExp), thenExp(thenExp), elseExp(elseExp) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class UnaryExpression : public AbstractExpression
{
public:
    char op;
    AbstractExpression* lhs;

    std::string OpToStr(char op)
    {
        switch (op)
        {
        case (char)'~': return "~";
        case (char)'*': return "*";
        default: return "undefined";
        }
    }
    std::string stringValue() override { return OpToStr(op) + lhs->stringValue(); };


    UnaryExpression(char op, AbstractExpression* lhs) : op(op), lhs(lhs) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class ArrayAccessExpression : public AbstractExpression
{
public:
    VariableExpression* varExp;
    AbstractExpression* accessIndex;

    std::string stringValue() override { return varExp->stringValue() + "[" + accessIndex->stringValue() + "]"; }

    ArrayAccessExpression(VariableExpression* varExp, AbstractExpression* accessIndex) : varExp(varExp), accessIndex(accessIndex) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class SetRefExpression : public AbstractExpression
{
public:
    VariableExpression* varExp;

    std::string stringValue() override { return "set " + varExp->stringValue(); }

    SetRefExpression(VariableExpression* varExp) : varExp(varExp) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class FunctionCallExpression : public AbstractExpression
{
public:
    std::string fnName;
    std::vector<AbstractExpression*> args;

    std::string stringValue() override {
        std::stringstream ss;
        ss << fnName;
        ss << "(";
        for (const auto& arg : args)
        {
            ss << arg->stringValue();
            if (arg != args.back())
            {
                ss << ", ";
            }
        }
        ss << ")";
        return ss.str();
    }

    FunctionCallExpression(std::string fnName, std::vector<AbstractExpression*> args) : fnName(fnName), args(args) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class AddressOfExpression : public AbstractExpression
{
public:
    AbstractExpression* lhs;

    std::string stringValue() override { return "&" + lhs->stringValue(); };


    AddressOfExpression(AbstractExpression* lhs) : lhs(lhs) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class NopExpression : public AbstractExpression
{
public:
    std::string stringValue() override { return "nop"; };


    NopExpression() {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};

class BinaryExpression : public AbstractExpression {
public:
    char op;
    AbstractExpression* lhs, * rhs;
    std::string OpToStr(char op)
    {
        switch (op)
        {
        case (char)BinaryExprType::BinAdd: return "+";
        case (char)BinaryExprType::BinSub: return "-";
        case (char)BinaryExprType::BinMod: return "%";
        case (char)BinaryExprType::BinLT: return "<";
        case (char)BinaryExprType::BinGT: return ">";
        case (char)BinaryExprType::BinLTE: return "<=";
        case (char)BinaryExprType::BinGTE: return ">=";
        case (char)BinaryExprType::BinEq: return "==";
        case (char)BinaryExprType::BinNEq: return "!=";
        case (char)BinaryExprType::BinMul: return "*";
        case (char)BinaryExprType::BinDiv: return "/";
        case (char)BinaryExprType::BitAnd: return "&";
        case (char)BinaryExprType::BitOr: return "|";
        case (char)BinaryExprType::BitXor: return "^";
        case (char)BinaryExprType::BitShl: return "<<";
        case (char)BinaryExprType::BitShr: return ">>";
        case (char)BinaryExprType::LogAnd: return "&&";
        case (char)BinaryExprType::LogOr: return "||";
        default: return "undefined";
        }
    }
    std::string stringValue() override { return lhs->stringValue() + " " + OpToStr(op) + " " + rhs->stringValue(); };


    BinaryExpression(char op, AbstractExpression* lhs, AbstractExpression* rhs) : op(op), lhs(lhs), rhs(rhs) {}
    void Accept(AbstractVisitor* visitor) override { visitor->Visit(this); }
};
#endif /* AbstractExpresssion_hpp */