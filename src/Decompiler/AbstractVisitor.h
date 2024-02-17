//
//  AbstractVisitor.hpp
//  Compiler
//
//  Created by Филипп Федяков on 05.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef AbstractVisitor_hpp
#define AbstractVisitor_hpp

#include <stdio.h>
class NumberExpression;
class VariableExpression;
class AssignExpression;
class IfExpression;
class ForExpression;
class BinaryExpression;
class AbstractExpression;
class AbstractStatement;
class BranchStatement;
class AssignStatement;
class StringExpression;
class ReturnStatement;
class ReturnExpression;
class GotoStatement;
class GotoExpression;
class UnaryExpression;
class AddressOfExpression;
class ArrayAccessExpression;
class SetRefExpression;
class FunctionCallExpression;
class NopExpression;
class AbortExpression;
class ExitExpression;
class AbortStatement;
class ExitStatement;
class PropPutExpression;
class PropGetExpression;

class AbstractVisitor {
public:
    virtual void Visit(NumberExpression* exp) = 0;
    virtual void Visit(StringExpression* exp) = 0;
    virtual void Visit(VariableExpression* exp) = 0;
    virtual void Visit(AssignExpression* exp) = 0;
    virtual void Visit(IfExpression* exp) = 0;
    virtual void Visit(ForExpression* exp) = 0;
    virtual void Visit(BinaryExpression* exp) = 0;
    virtual void Visit(ReturnExpression* exp) = 0;
    virtual void Visit(GotoExpression* exp) = 0;
    virtual void Visit(UnaryExpression* exp) = 0;
    virtual void Visit(AddressOfExpression* exp) = 0;
    virtual void Visit(ArrayAccessExpression* exp) = 0;
    virtual void Visit(SetRefExpression* exp) = 0;
    virtual void Visit(FunctionCallExpression* exp) = 0;
    virtual void Visit(NopExpression* exp) = 0;
    virtual void Visit(AbortExpression* exp) = 0;
    virtual void Visit(ExitExpression* exp) = 0;
    virtual void Visit(PropPutExpression* exp) = 0;
    virtual void Visit(PropGetExpression* exp) = 0;

    virtual void Visit(BranchStatement* stmt) = 0;
    virtual void Visit(AssignStatement* stmt) = 0;
    virtual void Visit(ReturnStatement* stmt) = 0;
    virtual void Visit(GotoStatement* stmt) = 0;
    virtual void Visit(ExitStatement* stmt) = 0;
    virtual void Visit(AbortStatement* stmt) = 0;
};
#endif /* AbstractVisitor_hpp */