#include "Function.h"
#include "Parser/Parser.h"
#include "IRGen/IRGenerator.h"
#include <algorithm>
#include <sstream>
#include "Frontend.h"
#include "Variables/SymbolTable.h"
#include "Variables/NumberVariable.h"
#include "Variables/StringVariable.h"
#include "Variables/VariantVariable.h"

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
	CVariable* var = nullptr;
	switch (typeInfo.internalType)
	{
	case ConcreteType::Number:
	case ConcreteType::NumberPointer:
		var = new CNumberVariable(-100 - (nNums + 1));
		nNums++;
		break;
	case ConcreteType::String:
	case ConcreteType::StringPointer:
		var = new CStringVariable(-100 - (nStrs + 1));
		nStrs++;
		break;
	case ConcreteType::Object:
	case ConcreteType::ObjectPointer:
		var = new CVariantVariable(-100 - (nObjs + 1));
		nObjs++;
		break;
	default:
		throw std::runtime_error("Invalid argument type info");
		break;
	}
	m_argVars.Add(var);
}

void CFunction::Construct(const std::vector<AbstractExpression*>& expressions)
{
	m_gen = new IRGenerator;

	for (std::vector<AbstractExpression*>::const_iterator it = expressions.begin(); it != expressions.end(); ++it)
	{
		auto expr = *it;
		//std::cout << "Construct GenerateIR: " << expr->stringValue() << std::endl;
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
				out << std::string(indentLvl, '\t') << s->Dump() << ';' << std::endl;
			}
		}
		else if (s->type == StatementType::GOTO)
		{
			auto gotoSt = dynamic_cast<GotoStatement*>(s);
			out << std::string(indentLvl, '\t') << "goto " << gotoSt->targetLabel << ';' << std::endl;
		}
		else
		{
			out << std::string(indentLvl, '\t') << s->Dump() << ';' << std::endl;
		}
	}
	visited[bb] = true;
}

void CFunction::SetGlobalSymTable(SymbolTable* symTable)
{
	/* Symbol tables go like this:
	 *  Global symtable -> Arguments symtable -> Locals symtable
	 */
	m_argVars.SetParent(symTable);
	m_localVars.SetParent(&m_argVars);
}

void CFunction::SetVariables(const CDataDeclList& declList)
{
	m_nLocalNums = declList.GetNumNumbers();
	m_nLocalStrs = declList.GetNumStrings();
	
	const size_t numOffset = m_nArgNums;
	for (size_t i = 0; i < declList.GetNumNumbers()-m_nArgNums; i++)
	{
		m_localVars.Add(new CNumberVariable(-100 - (i+1+numOffset)));
	}

	const size_t strOffset = m_nArgStrs;
	for (size_t i = 0; i < declList.GetNumStrings()-m_nArgStrs; i++)
	{
		std::optional<uint16_t> stringSize = std::nullopt;
		for (auto strInfo : declList.GetStringTable())
		{
			if (strInfo.varId == i)
			{
				stringSize = strInfo.stringSize;
			}
		}
		m_localVars.Add(new CStringVariable(-100 - (i+1+strOffset), stringSize));
	}

	const size_t varOffset = m_nArgObjs;
	auto objectTable = declList.GetObjectTable();
	for (size_t i = 0; i < objectTable.size()-m_nArgObjs; i++)
	{
		auto obj = objectTable[i];
		CVariantVariable* var = new CVariantVariable(-100 - (i + varOffset + 1));
		if (((int)obj.flags & (int)SymFlags::vbArray) != 0)
		{
			var->SetIsArray(true);
			var->SetElemCount(obj.elemCount);
		}
		else
		{
			if (obj.typedefId != -1)
			{
				var->SetIsStruct(true);
				var->SetTypedef(m_script->GetStruct(obj.typedefId));
			}
		}
		m_localVars.Add(var);
	}
}


std::ostream& operator<<(std::ostream& out, const CFunction& o)
{
	if (o.GetName().empty())
	{
		out << "function" << ' ' << o.GetReturnType() << ' ' << "Func_" << o.GetAddress() << '(';
	}
	else
	{
		out << "function" << ' ' << o.GetReturnType() << ' ' << o.GetName() << '(';
	}
	// print argument list
	auto args = o.m_argVars.GetVariables();
	for (auto it = args.begin(); it != args.end(); ++it)
	{
		out << **it;
		if (it != args.cend() - 1)
			out << ',';
	}
	out << ')' << std::endl;
	
	// print local variables
	for (auto var : o.m_localVars.GetVariables())
	{
		out << *var << ';' << std::endl;
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
