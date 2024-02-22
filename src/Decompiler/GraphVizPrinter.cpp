#include "GraphVizPrinter.h"
#include "CFG/ControlFlowGraph.h"
#include "CFG/BasicBlock.h"
#include <iostream>
#include "IRGen/IRStatements.h"

void GraphVizPrinter::print(std::ostream& os, const std::string fnName) {
    static int fnCounter = 1;
    std::string name = fnName;
    if (name.empty())
    {
        name = "Func_" + std::to_string(fnCounter++);
    }
    os << "digraph " << name << " {" << std::endl;
    for (auto bb : _cfg->basicBlocks) {
        os << "\tBB_" << bb->GetIndex() << "[shape = record, color = \"#b70d28ff\", style = filled, fillcolor = \"#b70d2870\", label = \"{" << "|{<s0>T|<s1>F}}\"]; " << std::endl;
        for (auto succ : bb->succs) {
            os << "\t\"" << "BB_" + std::to_string(bb->GetIndex()) << "\" -> \"" << "BB_" + std::to_string(succ->GetIndex()) << "\"" << std::endl;
        }
    }
    os << "}" << std::endl;
}