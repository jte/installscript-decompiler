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
	for (const auto& a : args)
	{
		AddArgument(a, m_nArgNums, m_nArgStrs, m_nArgObjs);
	}
}

void CFunction::AddArgument(ArgumentTypeInfo typeInfo, size_t& nNums, size_t& nStrs, size_t& nObjs)
{
	std::stringstream ss;
	switch (typeInfo.internalType)
	{
	case ConcreteType::Number:
	case ConcreteType::NumberPointer:
		ss << "LclVarNum" << nNums + 1;
		nNums++;
		break;
	case ConcreteType::String:
	case ConcreteType::StringPointer:
		ss << "LclVarStr" << nStrs + 1;
		nStrs++;
		break;
	case ConcreteType::Object:
	case ConcreteType::ObjectPointer:
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
		std::cout << "Construct GenerateIR: " << expr->stringValue() << std::endl;
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
		if (s->type == StatementType::BRANCH)
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
		else if (s->type == StatementType::GOTO)
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

void CFunction::SetVariables(const CDataDeclList& declList)
{
	m_declList = declList;
	m_nLocalNums = declList.GetNumNumbers();
	m_nLocalStrs = declList.GetNumStrings();
}

std::ostream& operator<<(std::ostream& out, const CFunction& o)
{
	out << "function" << ' ' << o.GetReturnType() << ' ' << o.GetName() << "Func_" << o.GetAddress() << '(';
	// print argument list
	for (auto it = o.m_arguments.begin(); it != o.m_arguments.end(); ++it)
	{
		out << (*it)->name;
		if (it != o.m_arguments.cend() - 1)
			out << ',';
	}
	out << ')' << std::endl;
	
	// print local variables
	for (size_t i = 0; i < (o.m_nLocalNums - o.m_nArgNums); i++)
	{
		out << "INT LclVarNum" << o.m_nArgNums + 1 + i << ';' << std::endl;
	}
	for (size_t i = 0; i < (o.m_nLocalStrs - o.m_nArgStrs); i++)
	{
		out << "STRING LclVarStr" << o.m_nArgStrs + 1 + i;
		for (auto strInfo : o.m_declList.GetStringTable())
		{
			if (strInfo.varId == i)
			{
				out << "[" << strInfo.stringSize << "]";
			}
		}
		out << ';';
		out << std::endl;
	}
	auto objectTable = o.m_declList.GetObjectTable();
	size_t variantId = 1;
	for (auto obj : objectTable)
	{
		if (((int)obj.flags & (int)SymFlags::vbArray) != 0)
		{
			out << "VARIANT LclVarObj" << variantId << "(" << obj.elemCount << ");" << std::endl;
		}
		else
		{
			if (obj.typedefId == -1)
			{
				out << "VARIANT LclVarObj" << variantId << ';' << std::endl;
			}
			else
			{
				out << "STRUCT_" << obj.typedefId + 1 << " LclVarObj" << variantId << ';' << std::endl;
			}
		}
		variantId++;
	}
	// print function body
	out << "begin" << std::endl;
	
	if (o.m_gen->m_cfg)
	{
		//o.m_gen->m_cfg->CommitAllChanges(); throws error, probably because of abuse of cfg
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
