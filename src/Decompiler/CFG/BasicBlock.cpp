﻿//
//  BaseBlock.cpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "BasicBlock.h"
using namespace std;

void BasicBlock::AddStatement(AbstractStatement* statement) {
    statements.push_back(statement);
}

void BasicBlock::AddLink(BasicBlock* pred, BasicBlock* succ) {
    if (std::find(succ->preds.begin(), succ->preds.end(), pred) == succ->preds.end()) {
        succ->preds.push_back(pred);
    }

    if (std::find(pred->succs.begin(), pred->succs.end(), succ) == pred->succs.end()) {
        pred->succs.push_back(succ);
    }
}

std::string BasicBlock::stringValue() {
    return "bb #" + std::to_string(_index) + " " + _label;
}

std::string BasicBlock::GetLabel() {
    return _label;
}