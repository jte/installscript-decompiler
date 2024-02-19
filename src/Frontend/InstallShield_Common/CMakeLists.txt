##
##

set (SOURCES
	IArgument.h
	IArgument.cpp
	NumConst.h
	NumConst.cpp
	StrConst.h
	StrConst.cpp
	StreamPtr.h
	StreamPtr.cpp
	Type.h
	Type.cpp
	vararg.h
	vararg.cpp
	HeaderKind.h
	Frontend.h
	Frontend.cpp
	DataDeclList.cpp
    DataDeclList.h
	ISBasicBlock.cpp
    ISBasicBlock.h
	ActionFile/ActionFile.h
	ActionFile/ActionFile.cpp
	Action.h
	Action.cpp
	ScriptFunction.h
	ScriptFunction.cpp
	ScriptPrototype.h
	ScriptPrototype.cpp
)

source_group("" FILES ${SOURCES})

add_library(InstallShield_Common STATIC ${SOURCES})

target_include_directories(InstallShield_Common PRIVATE ${ISIS_BUILD_DIR}/src/Decompiler)