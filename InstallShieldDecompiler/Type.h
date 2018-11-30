#pragma once

#include <string>

enum class ArgType
{
	StrArg,
	NumArg,
	VariantArg
};

enum class ScriptType : uint8_t
{
	String = 0,
	Char = 1,
	Short = 2,
	Int = 3,
	Long = 4,
	Pointer = 5,
	Bool = 6,
	Hwnd = 7,
	Void = 8,
	Struct = 9,
	Object = 10,
	Binary = 11,
	WString = 12,
	WPointer = 13
};

enum class ConcreteType : uint8_t
{
	StringPointer = 2,
	NumberPointer = 3,
	ObjectPointer = 4,
	String = 5,
	Number = 6,
	Object = 7
};

struct ArgumentTypeInfo
{
	ScriptType scriptType;
	ConcreteType internalType;
};

std::ostream& operator<<(std::ostream& out, const ScriptType& o);
std::ostream& operator<<(std::ostream& out, const ConcreteType& o);
std::ostream& operator<<(std::ostream& out, const ArgType& o);