#include "Type.h"
#include "StreamPtr.h"

std::ostream& operator<<(std::ostream& out, const ArgType& o)
{
	switch (o)
	{
	case ArgType::StrArg:
		out << "{CStrArg}";
		break;
	case ArgType::NumArg:
		out << "{CNumArg}";
		break;
	case ArgType::VariantArg:
		out << "{CVariantArg}";
		break;
	default:
		throw std::runtime_error("Unknown argument type " + std::to_string(static_cast<int32_t>(o)));
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const ConcreteType& o)
{
	switch (o)
	{
	case ConcreteType::Number: 
		out << "NUM";
		break;
	case ConcreteType::Object:
		out << "OBJ";
		break;
	case ConcreteType::String:
		out << "STR";
		break;
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const ScriptType& o)
{
	switch (o)
	{
	case ScriptType::String:
		out << "STRING";
		break;
	case ScriptType::Char:
		out << "CHAR";
		break;
	case ScriptType::Short:
		out << "SHORT";
		break;
	case ScriptType::Int:
		out << "INT";
		break;
	case ScriptType::Long:
		out << "LONG";
		break;
	case ScriptType::Pointer:
		out << "POINTER";
		break;
	case ScriptType::Bool:
		out << "BOOL";
		break;
	case ScriptType::Hwnd:
		out << "HWND";
		break;
	case ScriptType::Void:
		out << "VOID";
		break;
	case ScriptType::Struct:
		out << "STRUCT";
		break;
	case ScriptType::Object:
		out << "OBJECT";
		break;
	case ScriptType::Binary:
		out << "BINARY";
		break;
	case ScriptType::WString:
		out << "WSTRING";
		break;
	case ScriptType::WPointer:
		out << "WPOINTER";
		break;
	default:
		throw std::runtime_error("Invalid type");
	}
	return out;
}