#pragma once

#include <Windows.h>
#include <cstdint>

typedef struct
{
	uint32_t _unknown1;
	uint32_t debugFlags;
	void *callerAppContext;
	uint16_t maxErrorCount;
	uint16_t maxWarningCount;
	uint16_t warningLevel;
	uint16_t __align;
	wchar_t *envIncludePath;
	HINSTANCE instance;
	HANDLE heap;
} CompilerContext;

enum PfnId
{
	PfnOnParseScriptProgress = 21,
	PfnOnReportMessage
};

enum InfoId
{
	InfoCallerApp,
	InfoMaxWarningCount,
	InfoMaxErrorCount,
	InfoEnvIncludePath,
	InfoCompilerFileVersion,
	InfoWarningLevel,
	InfoObsoleteFile
};

enum DebugFlags
{
	DebugFlagCreateDebugInfo,
	DebugFlagEmbedDebugInfo
};

typedef CompilerContext* (__stdcall *InstrcCreate)(HINSTANCE instance, int);
typedef int (__stdcall *InstrcCompile)(CompilerContext* compilerContext, const wchar_t *scriptFile, const wchar_t *outputFile, int debugFlags);
typedef BOOL (__stdcall *InstrcSetInfo)(CompilerContext* compilerContext, int infoId, void* info);
typedef void* (__stdcall *InstrcQueryInfo)(CompilerContext* compilerContext, int infoId);
typedef BOOL (__stdcall *InstrcSetPfn)(CompilerContext* compilerContext, int fnId, void *callback);
typedef BOOL (__stdcall *InstrcDefineSym)(CompilerContext* compilerContext, const char *symDef);
typedef BOOL (__stdcall *InstrcDestroy)(CompilerContext* compilerContext);
typedef BOOL (__stdcall *InstrcStop)();
typedef uint32_t (__stdcall *InstrcGetErrorCount)();
typedef int (__stdcall *InstrcLink)(CompilerContext* compilerContext, void *a2, int a3, int debuggingInfoFlags);
typedef int (__stdcall *InstrcCreateLib)(CompilerContext* compilerContext, void *a2, int arg8, int a4);
typedef int (__stdcall *InstrcGetWarnCount)();

typedef void (__cdecl *pfnOnReportMessage)(CompilerContext* compilerContext, void* callerAppContext, UINT msgId, wchar_t* fileName, wchar_t* rsrcErrorMessage, int lineNo, int posOffset, int);
// percentCompleted = 100 * (file_pos / file_size)
typedef void (__cdecl *pfnOnParseScriptProgress)(CompilerContext* compilerContext, void* callerAppContext, int percentCompleted, int);

struct CompilerModule
{
	HMODULE hDll;
	InstrcCreate pfnInstrcCreate;
	InstrcCompile pfnInstrcCompile;
	InstrcSetInfo pfnInstrcSetInfo;
	InstrcQueryInfo pfnInstrcQueryInfo;
	InstrcSetPfn pfnInstrcSetPfn;
	InstrcDefineSym pfnInstrcDefineSym;
	InstrcDestroy pfnInstrcDestroy;
	InstrcStop pfnInstrcStop;
	InstrcGetErrorCount pfnInstrcGetErrorCount;
	InstrcLink pfnInstrcLink;
	InstrcCreateLib pfnInstrcCreateLib;
	InstrcGetWarnCount pfnInstrcGetWarnCount;
};

CompilerModule* InitializeCompilerDll();