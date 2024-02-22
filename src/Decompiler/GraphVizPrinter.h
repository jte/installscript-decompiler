//
//  GraphVizPrinter.hpp
//  Compiler
//
//  Created by Филипп Федяков on 05.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef GraphVizPrinter_hpp
#define GraphVizPrinter_hpp

#include <stdio.h>
#include <string>
#include <ostream>

class ControlFlowGraph;


class GraphVizPrinter {
    ControlFlowGraph* _cfg;

public:
    void print(std::ostream& os, const std::string fnName);
    GraphVizPrinter(ControlFlowGraph* cfg) : _cfg(cfg) {}
};

#endif /* GraphVizPrinter_hpp */