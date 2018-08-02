#include "CompilerDLL.h"
#include <Windows.h>

CompilerModule* InitializeCompilerDll()
{
	static CompilerModule fns;
	HMODULE hCompiler = LoadLibraryW(L"Compiler.dll");
	if (!hCompiler)
	{
		return nullptr;
	}
	fns.hDll = hCompiler;
	fns.pfnInstrcCreate = (InstrcCreate)GetProcAddress(hCompiler, "InstrcCreate");
	fns.pfnInstrcCompile = (InstrcCompile)GetProcAddress(hCompiler, "InstrcCompile");
	fns.pfnInstrcSetPfn = (InstrcSetPfn)GetProcAddress(hCompiler, "InstrcSetPfn");
	return &fns;
}