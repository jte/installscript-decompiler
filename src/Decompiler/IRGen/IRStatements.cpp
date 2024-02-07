//
//  IRStatements.cpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "IRStatements.h"
#include "Parser/Expressions.h"
#include "CFG/BasicBlock.h"

// TODO: Classes for variables and values, not to use Expressions

std::string AssignStatement::Dump() 
{
    return var->stringValue() + " = " + (rhs ? rhs->stringValue() : "null");
}

std::string BranchStatement::Dump() 
{
    if (isConditional) {
        return "branch on: " + (condition ? condition->stringValue() : "null") + " to: " + firstBranchBB->stringValue() + " or: " + (secondBranchBB ? secondBranchBB->stringValue() : "null") + "\n";
    }
    else {
        return "goto " + firstBranchBB->stringValue();
    }
}

std::string ReturnStatement::Dump()
{
    return "return" + (exp ? (" " + exp->stringValue()) : "");
}

std::string GotoStatement::Dump()
{
    return "goto " + targetExp->stringValue();
}

std::string AbortStatement::Dump()
{
    return "abort";
}

std::string ExitStatement::Dump()
{
    return "exit";
}