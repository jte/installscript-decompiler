#include "Function.h"
#include <algorithm>

uint32_t CFunction::GetAddress() const
{
	return m_address;
}

void CFunction::AddBasicBlock(CBasicBlock* bb)
{
	m_bbs.push_back(bb);
	GatherVariables(*bb);
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
		AddArgument(a);
	}
	m_arguments = args;
}

void CFunction::AddArgument(ArgumentTypeInfo typeInfo)
{
	switch (typeInfo.internalType)
	{
	case ConcreteType::Number:
		m_numbers.push_back(new CVariable(ArgType::NumArg, -1 - m_numbers.size()));
	break;
	case ConcreteType::String:
		m_strings.push_back(new CVariable(ArgType::StrArg, -1 - m_strings.size()));
	break;
	case ConcreteType::Object:
		m_objects.push_back(new CVariable(ArgType::VariantArg, -1 - m_objects.size()));
	break;
	default:
		throw std::runtime_error("Invalid argument type info");
		break;
	}
}


std::ostream& operator<<(std::ostream& out, const CFunction& o)
{
	out << "function" << ' ' << o.GetReturnType() << ' ' << o.GetName() << '@' << o.GetAddress() << '(';
	// print argument list
	auto numIt = o.m_numbers.begin();
	auto objIt = o.m_objects.begin();
	auto strIt = o.m_strings.begin();
	for (auto it = o.m_arguments.cbegin(); it != o.m_arguments.cend(); ++it)
	{
		out << 'a';
		switch ((*it).internalType)
		{
		case ConcreteType::Number:
			out << *numIt;
			++numIt;
		break;
		case ConcreteType::Object:
			out << *objIt;
			++objIt;
		break;
		case ConcreteType::String:
			out << *strIt;
			++strIt;
		break;
		}
		if (it != o.m_arguments.cend() - 1)
			out << ',';
	}
	out << ')' << std::endl;
	// print local variables
	for (; numIt != o.m_numbers.end(); ++numIt)
	{
		out << 'v' << *numIt << std::endl;
	}
	for (; strIt != o.m_strings.end(); ++strIt)
	{
		out << 'v' << *strIt << std::endl;
	}
	for (; objIt != o.m_objects.end(); ++objIt)
	{
		out << 'v' << *objIt << std::endl;
	}
	out << "begin" << std::endl;
	// print bbs
	for (auto bb = o.m_bbs.cbegin(); bb != o.m_bbs.cend(); ++bb)
	{
		auto label = (*bb)->GetAddress();
		if (label != 0)
			out << "label_" << std::to_string((*bb)->GetAddress()) << ':' << std::endl;
		out << *(*bb) << std::endl;
	}
	out << "end;";
	return out;
}

void CFunction::GatherVariables(const CBasicBlock& bb)
{
	for (auto& s : bb.GetStatements())
	{
		auto exprs = s.GetExpressions();
		for (auto expr : exprs)
		{
			CVariable* var = dynamic_cast<CVariable*>(expr);
			if (var != nullptr && var->IsLocal())
			{
				AddVariable(var);
			}
		}
	}
}

void CFunction::AddVariable(CVariable* var)
{
	switch (var->GetConcreteType())
	{
	case ArgType::NumArg:
		if (std::find_if(m_numbers.begin(), m_numbers.end(), [var](CVariable* cur) {return *cur == *var; }) == m_numbers.end())
			m_numbers.push_back(var);
		break;
	case ArgType::StrArg:
		if (std::find_if(m_strings.begin(), m_strings.end(), [var](CVariable* cur) {return *cur == *var; }) == m_strings.end())
			m_strings.push_back(var);
		break;
	case ArgType::VariantArg:
		if (std::find_if (m_objects.begin(), m_objects.end(), [var](CVariable* cur) {return *cur == *var; }) == m_objects.end())
			m_objects.push_back(var);
		break;
	}
}

//bool VariableExists(CVariable* var);