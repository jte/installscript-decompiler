#include "Function.h"
#include "Parser/Parser.h"
#include "IRGen/IRGenerator.h"
#include <algorithm>
#include <sstream>

uint32_t CFunction::GetAddress() const
{
	return m_address;
}


std::string CFunction::GetName() const
{
	return m_name;
}

ScriptType CFunction::GetReturnType() const
{
	return m_returnType;
}

void CFunction::SetArguments(std::vector<ArgumentTypeInfo> args)
{
	size_t nNums = 0, nStrs = 0, nObjs = 0;
	for (const auto& a : args)
	{
		AddArgument(a, nNums, nStrs, nObjs);
	}
}

void CFunction::AddArgument(ArgumentTypeInfo typeInfo, size_t& nNums, size_t& nStrs, size_t& nObjs)
{
	std::stringstream ss;
	switch (typeInfo.internalType)
	{
	case ConcreteType::Number:
		ss << "LclVarNum" << nNums + 1;
		nNums++;
		break;
	case ConcreteType::String:
		ss << "LclVarStr" << nStrs + 1;
		nStrs++;
		break;
	case ConcreteType::Object:
		ss << "LclVarObj" << nObjs + 1;
		nObjs++;
		break;
	default:
		throw std::runtime_error("Invalid argument type info");
		break;
	}
	m_arguments.push_back(new VariableExpression(ss.str()));
}

void CFunction::Construct(const std::vector<AbstractExpression*>& expressions)
{
	m_gen = new IRGenerator;

	for (std::vector<AbstractExpression*>::const_iterator it = expressions.begin(); it != expressions.end(); ++it)
	{
		auto expr = *it;
		m_gen->GenerateIR(expr);
	}
}

void PrintBB(std::ostream& out, BasicBlock *bb, std::map<BasicBlock*,bool>& visited, size_t indentLvl)
{
	if (visited[bb])
		return;
	for (const auto& s : bb->statements)
	{
		if (s->displayLabel != "")
		{
			out << std::string(indentLvl, '\t') << s->displayLabel << ":" << std::endl;
		}
		if (s->type == StatementTypeX::BRANCH)
		{
			auto branchSt = dynamic_cast<BranchStatement*>(s);
			if (branchSt->isConditional) {
				out << std::string(indentLvl, '\t') << "if " << branchSt->condition->stringValue() << " then" << std::endl;
				PrintBB(out, branchSt->firstBranchBB, visited, indentLvl + 1);
				visited[branchSt->firstBranchBB] = true;
				out << std::string(indentLvl, '\t') << "endif;" << std::endl;
			}
			else
			{
				out << std::string(indentLvl, '\t') << s->Dump() << std::endl;
			}
		}
		else if (s->type == StatementTypeX::GOTO)
		{
			auto gotoSt = dynamic_cast<GotoStatement*>(s);
			out << std::string(indentLvl, '\t') << "goto " << gotoSt->targetLabel << std::endl;
		}
		else
		{
			out << std::string(indentLvl, '\t') << s->Dump() << std::endl;
		}
	}
	visited[bb] = true;
}

std::ostream& operator<<(std::ostream& out, const CFunction& o)
{
	out << "function" << ' ' << o.GetReturnType() << ' ' << o.GetName() << '@' << o.GetAddress() << '(';
	// print argument list
	for (auto it = o.m_arguments.begin(); it != o.m_arguments.end(); ++it)
	{
		out << (*it)->name;
		if (it != o.m_arguments.cend() - 1)
			out << ',';
	}
	out << ')' << std::endl;
	out << "begin" << std::endl;
	
	if (o.m_gen->m_cfg)
	{
		o.m_gen->m_cfg->CommitAllChanges();
		std::map<BasicBlock*, bool> visited;
		for (const auto& bb : o.m_gen->m_cfg->basicBlocks) 
		{
			PrintBB(out, bb, visited, 0);
		}
	}
	out << "end;" << std::endl;
	
	//o.m_gen->GetGraphVizPrinter().print();
	
	return out;
}
