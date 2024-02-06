#include "Decompiler.h"
#include "Prototype/InternalPrototype.h"
#include "Function.h"
#include "IScript.h"
#include <algorithm>
#include <vector>
#include "Parser/Parser.h"

CDecompiler::CDecompiler(const CIScript& script) : m_script(script)
{
	auto fns = script.GetFns();
	for (const auto& fn : fns)
	{
		if (fn.bbs.size() == 0)
			continue; // skip predefined funcs
		auto& outFn = AddFunctionPrototype(fn.prototype);
		Parser* parser = new Parser;
		std::vector<AbstractExpression*> expressions = parser->Parse(fn.bbs);
		outFn.Construct(expressions);
		delete parser;
	}
}

CFunction& CDecompiler::AddFunctionPrototype(CPrototype* proto)
{
	CInternalPrototype *iproto = dynamic_cast<CInternalPrototype*>(proto);
	if (iproto)
	{
		
		if (iproto->GetIsExported())
		{
			CFunction fn(iproto->GetName(), iproto->GetBBId(), iproto->GetReturnType());
			m_functions.push_back(fn);
		}
		else
		{
			CFunction fn(iproto->GetBBId(), iproto->GetReturnType());
			m_functions.push_back(fn);
		}
		CFunction& f = m_functions.back();
		f.SetArguments(iproto->GetArgumentTypeInfo());
		return f;
	}
	throw std::runtime_error("Invalid prototype");
}

CFunction& CDecompiler::GetFunction(size_t address)
{
	auto found = std::find_if(m_functions.begin(), m_functions.end(), [address] (CFunction& func) {
		return func.GetAddress() == address;
	});
	if (found == m_functions.end())
	{
		throw std::runtime_error("Could not find function prototype with address " + std::to_string(address));
	}
	return *found;
}

std::ostream& operator<<(std::ostream& out, const CDecompiler& o)
{
	for (auto s : o.m_script.GetStructs())
	{
		out << s << std::endl;
	}
	for (const auto &fn : o.m_functions)
	{
		out << fn << std::endl;
	}
	return out;
}