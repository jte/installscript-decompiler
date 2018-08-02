#include <iostream>
#include <string>
#include <Windows.h>
#include "CompilerDLL.h"

void OnReportMessage(CompilerContext* compilerContext, void* compileContext, UINT msgId, wchar_t* fileName, wchar_t* errorMessage, int lineNo, int posOffset, int unused)
{
	std::wcout << L"OnReportError(.msgId = " << msgId << ", .fileName = " << fileName << ", .err = " << errorMessage << ", .line = " << lineNo << ", pos = " << posOffset << ",unused =" << unused << ");" << std::endl;
}

void OnParseScriptProgress(CompilerContext* compilerContext, void* compileContext, int percentCompleted, int)
{
	std::wcout << L"Compiling... progress " << percentCompleted << "% / 100%" << std::endl;
}

int main(int argc, char** argv)
{
	CompilerModule *fns = InitializeCompilerDll();
	if (!fns)
	{
		std::wcout << L"Could not acquire Compiler.dll" << std::endl;
		return 1;
	}
	CompilerContext *compilerContext = fns->pfnInstrcCreate(fns->hDll, 0);
	fns->pfnInstrcSetPfn(compilerContext, PfnOnReportMessage, OnReportMessage);
	fns->pfnInstrcSetPfn(compilerContext, PfnOnParseScriptProgress, OnParseScriptProgress);
	if (argc == 2 && argv[1] == "/ida")
	{
		fns->pfnInstrcCompile(compilerContext, L"script.rul", L"output.obs", 0);
	}
	else
	{
		std::string cmd = "exit";
		do
		{
			std::cout << "Do you want to compile (type 'exit' to quit) ? ";
			std::cin >> cmd;
			fns->pfnInstrcCompile(compilerContext, L"script.rul", L"output.obs", 0);
		} while (cmd != "exit");
	}
	return 0;
}