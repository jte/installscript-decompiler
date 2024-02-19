//
//  Parser.hpp
//  Compiler
//
//  Created by Филипп Федяков on 27.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <vector>
#include "../../InstallShield/ISBasicBlock.h"
#include "Variables/SymbolTable.h"

class AbstractExpression;
class CAction;

class Parser {
public:
    AbstractExpression* ParseCurrentExpression(CAction* act, SymbolTable* symTable);
    std::vector<AbstractExpression*> Parse(const std::vector<ISBasicBlock>& isBBs, SymbolTable* symTable);
};