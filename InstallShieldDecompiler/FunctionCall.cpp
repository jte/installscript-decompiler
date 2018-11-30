#include "FunctionCall.h"
#include <string>
#include "Prototype.h"
#include "InternalPrototype.h"
#include "DLLPrototype.h"
#include "Literal.h"

void CFunctionCallExpression::print(std::ostream& out)
{
	const CInternalPrototype* iproto = dynamic_cast<const CInternalPrototype*>(m_proto);
	auto name = iproto->GetName();
	if (name == "__LoadIScriptString")
	{
		CStringLiteral* str = dynamic_cast<CStringLiteral*>(m_args[0]);
		out << '@' << str->GetValue();
		return;
	}
	if (iproto->GetIsExported() || !name.empty())
	{
		out << iproto->GetName();
	}
	else
	{
		out << "@" << iproto->GetBBId();
	}
	out << '(';
	for (auto arg = m_args.cbegin(); arg != m_args.cend(); ++arg)
	{
		out << *arg;
		if (arg != m_args.cend() - 1)
			out << ", ";
	}
	out << ')';
}