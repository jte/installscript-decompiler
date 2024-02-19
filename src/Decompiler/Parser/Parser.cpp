//
//  Parser.cpp
//  Compiler
//
//  Created by Филипп Федяков on 27.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "Parser.h"
#include "Expressions.h"
#include "InstallShield_New/Actions/IfAction.h"
#include "InstallShield_New/Actions/GotoAction.h"
#include <string>
#include <map>
#include <cassert>
#include "Function.h"
#include "ISBasicBlock.h"

AbstractExpression* Parser::ParseCurrentExpression(CAction* act, SymbolTable* symTable) {
    auto expr = act->ToExpression(symTable);
    return expr;
}

std::vector<AbstractExpression*> Parser::Parse(const std::vector<ISBasicBlock>& isBBs, SymbolTable* symTable) {
    std::vector<AbstractExpression*> expressions;

    AbstractExpression* expr = nullptr;
    std::map<CAction*, AbstractExpression*> map;
    std::vector<size_t> ifExprs;
    std::vector<size_t> gotoExprs;
    int labelCounter = 1;

    for (const auto& bb : isBBs)
    {
        for (const auto& act : bb.GetActions())
        {
            //act->SetBBId(bb.GetBBId());

            expr = ParseCurrentExpression(act, symTable);

            if (expr) {
                if (dynamic_cast<IfExpression*>(expr))
                {
                    ifExprs.push_back(expressions.size());
                }
                if (dynamic_cast<GotoExpression*>(expr))
                {
                    gotoExprs.push_back(expressions.size());
                }
                if (expressions.size() != 0)
                    expressions.back()->next = expr;
                expressions.push_back(expr);
                map[act] = expr;
            }
        }
    }
    size_t lastIfExpr = 0;
    size_t lastGotoExpr = 0;
    for (size_t i = 0; i < isBBs.size(); i++)
    {
        const ISBasicBlock& bb = isBBs[i];
        auto acts = bb.GetActions();
        for (size_t actId = 0; actId < acts.size(); actId++)
        {
            auto act = acts[actId];
            if (auto ifAct = dynamic_cast<newis::CIfAction*>(act))
            {
                auto elseOffset = ifAct->GetElseBranchLabelOffset();
                auto elseExpr = map[isBBs[i + elseOffset].GetActions()[0]];

                if (!elseExpr)
                {
                    throw std::runtime_error("Invalid else expression found");
                }

                AbstractExpression* thenExpr = nullptr;
                
                // check required because of potential empty ifs
                if (actId + 1 < acts.size())
                {
                    thenExpr = map[acts[actId + 1]];

                    if (!thenExpr)
                    {
                        throw std::runtime_error("Invalid then expression found");
                    }
                }
                
                auto ifExpr = dynamic_cast<IfExpression*>(expressions[ifExprs[lastIfExpr]]);
                ifExpr->thenExp = thenExpr;
                ifExpr->elseExp = elseExpr;

                lastIfExpr++;
            }
            else if (auto gotoAct = dynamic_cast<newis::CGotoAction*>(act))
            {
                auto targetOffset = gotoAct->GetLabelOffset();
                auto gacts = isBBs[i + targetOffset].GetActions();
                auto gact = gacts[0];
                auto targetExpr = map[gact];

                if (!targetExpr)
                {
                    throw std::runtime_error("Invalid goto target expression found");
                }

                auto gotoExpr = dynamic_cast<GotoExpression*>(expressions[gotoExprs[lastGotoExpr]]);
                gotoExpr->targetExp = targetExpr;

                // Set label if not already set
                if (targetExpr->displayLabel == "")
                    targetExpr->displayLabel = "label_" + std::to_string(labelCounter++);
                gotoExpr->targetLabel = targetExpr->displayLabel;

                lastGotoExpr++;
            }
        }
    }

    return expressions;
}