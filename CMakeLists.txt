cmake_minimum_required(VERSION 3.10)


message(STATUS "CMAKE Version: ${CMAKE_VERSION}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message(STATUS "Source Dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# Project name
project(InstallScriptDecompiler VERSION 1.0)

set(default_build_type "Debug")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
        STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
                 "Debug" "Release")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ISIS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ISIS_OUTPUT_DIR ${ISIS_BUILD_DIR}/bin/$<IF:$<CONFIG:Debug>,Debug,Release> CACHE STRING "Output directory for build artifacts")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ISIS_BUILD_DIR}/obj/$<IF:$<CONFIG:Debug>,Debug,Release>)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ISIS_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ISIS_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${ISIS_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ISIS_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${ISIS_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ISIS_OUTPUT_DIR})

set(INSTALLSHIELD_FILES
    src/InstallShield/CNumConst.cpp
    src/InstallShield/CNumConst.h
    src/InstallShield/CStrConst.cpp
    src/InstallShield/CStrConst.h
    src/InstallShield/DataDeclList.cpp
    src/InstallShield/DataDeclList.h
    src/InstallShield/IArgument.cpp
    src/InstallShield/IArgument.h
    src/InstallShield/ISBasicBlock.cpp
    src/InstallShield/ISBasicBlock.h
    src/InstallShield/IScript.cpp
    src/InstallShield/IScript.h
    src/InstallShield/StreamPtr.cpp
    src/InstallShield/StreamPtr.h
    src/InstallShield/Type.cpp
    src/InstallShield/Type.h
    src/InstallShield/vararg.cpp
    src/InstallShield/vararg.h
)

set(ACTION_FILE_FILES
    src/InstallShield/ActionFile/ActionFile.cpp
    src/InstallShield/ActionFile/ActionFile.h
)

set(ACTIONS_FILES
    src/InstallShield/Actions/AbortAction.cpp
    src/InstallShield/Actions/AbortAction.h
    src/InstallShield/Actions/Action.cpp
    src/InstallShield/Actions/Action.h
    src/InstallShield/Actions/ActionWithArgs.cpp
    src/InstallShield/Actions/ActionWithArgs.h
    src/InstallShield/Actions/AddressOfAction.cpp
    src/InstallShield/Actions/AddressOfAction.h
    src/InstallShield/Actions/AddressOfWideAction.cpp
    src/InstallShield/Actions/AddressOfWideAction.h
    src/InstallShield/Actions/AssignAction.cpp
    src/InstallShield/Actions/AssignAction.h
    src/InstallShield/Actions/BinaryAction.cpp
    src/InstallShield/Actions/BinaryAction.h
    src/InstallShield/Actions/BindVariableAction.cpp
    src/InstallShield/Actions/BindVariableAction.h
    src/InstallShield/Actions/DLLFuncCallAction.cpp
    src/InstallShield/Actions/DLLFuncCallAction.h
    src/InstallShield/Actions/EndCatchAction.cpp
    src/InstallShield/Actions/EndCatchAction.h
    src/InstallShield/Actions/EndFuncAction.cpp
    src/InstallShield/Actions/EndFuncAction.h
    src/InstallShield/Actions/EndTryAction.cpp
    src/InstallShield/Actions/EndTryAction.h
    src/InstallShield/Actions/ExitAction.cpp
    src/InstallShield/Actions/ExitAction.h
    src/InstallShield/Actions/FuncCallAction.cpp
    src/InstallShield/Actions/FuncCallAction.h
    src/InstallShield/Actions/FuncPrologAction.cpp
    src/InstallShield/Actions/FuncPrologAction.h
    src/InstallShield/Actions/GetByteAction.cpp
    src/InstallShield/Actions/GetByteAction.h
    src/InstallShield/Actions/GotoAction.cpp
    src/InstallShield/Actions/GotoAction.h
    src/InstallShield/Actions/IfAction.cpp
    src/InstallShield/Actions/IfAction.h
    src/InstallShield/Actions/InternalFuncCallAction.cpp
    src/InstallShield/Actions/InternalFuncCallAction.h
    src/InstallShield/Actions/NOPAction.cpp
    src/InstallShield/Actions/NOPAction.h
    src/InstallShield/Actions/PropGetAction.cpp
    src/InstallShield/Actions/PropGetAction.h
    src/InstallShield/Actions/PropPutAction.cpp
    src/InstallShield/Actions/PropPutAction.h
    src/InstallShield/Actions/PropPutRefAction.cpp
    src/InstallShield/Actions/PropPutRefAction.h
    src/InstallShield/Actions/ReturnAction.cpp
    src/InstallShield/Actions/ReturnAction.h
    src/InstallShield/Actions/SetByteAction.cpp
    src/InstallShield/Actions/SetByteAction.h
    src/InstallShield/Actions/TryAction.cpp
    src/InstallShield/Actions/TryAction.h
    src/InstallShield/Actions/UnaryAction.cpp
    src/InstallShield/Actions/UnaryAction.h
    src/InstallShield/Actions/UnUseDLLAction.cpp
    src/InstallShield/Actions/UnUseDLLAction.h
    src/InstallShield/Actions/UseDLLAction.cpp
    src/InstallShield/Actions/UseDLLAction.h
	src/InstallShield/Actions/StrLengthCharsAction.h
	src/InstallShield/Actions/StrSubAction.h
	src/InstallShield/Actions/StrFindAction.h
	src/InstallShield/Actions/StrCompareAction.h
	src/InstallShield/Actions/StrToNumAction.h
	src/InstallShield/Actions/NumToStrAction.h
	src/InstallShield/Actions/HandlerAction.h
	src/InstallShield/Actions/HandlerExAction.h
	src/InstallShield/Actions/DoHandlerAction.h
	src/InstallShield/Actions/ResizeAction.h
	src/InstallShield/Actions/SizeofAction.h
	src/InstallShield/Actions/StrLengthCharsAction.cpp
	src/InstallShield/Actions/StrSubAction.cpp
	src/InstallShield/Actions/StrFindAction.cpp
	src/InstallShield/Actions/StrCompareAction.cpp
	src/InstallShield/Actions/StrToNumAction.cpp
	src/InstallShield/Actions/NumToStrAction.cpp
	src/InstallShield/Actions/HandlerAction.cpp
	src/InstallShield/Actions/HandlerExAction.cpp
	src/InstallShield/Actions/DoHandlerAction.cpp
	src/InstallShield/Actions/ResizeAction.cpp
	src/InstallShield/Actions/SizeofAction.cpp
	src/InstallShield/Actions/IndirectStructAction.cpp
	src/InstallShield/Actions/IndirectStructAction.h
)

set(PROTOTYPE_FILES
    src/InstallShield/Prototype/DLLPrototype.cpp
    src/InstallShield/Prototype/DLLPrototype.h
    src/InstallShield/Prototype/InternalPrototype.cpp
    src/InstallShield/Prototype/InternalPrototype.h
    src/InstallShield/Prototype/Prototype.cpp
    src/InstallShield/Prototype/Prototype.h
)

set(STRUCT_FILES
    src/InstallShield/Struct/Struct.cpp
    src/InstallShield/Struct/Struct.h
)

set(WRAPPER_FILES
    src/InstallShield/Wrapper/LibFile.cpp
    src/InstallShield/Wrapper/LibFile.h
)

source_group("InstallShield" FILES ${INSTALLSHIELD_FILES})

list(APPEND INSTALLSHIELD_FILES ${ACTION_FILE_FILES} ${ACTIONS_FILES} ${PROTOTYPE_FILES} ${STRUCT_FILES} ${WRAPPER_FILES})

source_group("InstallShield/ActionFile" FILES ${ACTION_FILE_FILES})
source_group("InstallShield/Actions" FILES ${ACTIONS_FILES})
source_group("InstallShield/Prototype" FILES ${PROTOTYPE_FILES})
source_group("InstallShield/Struct" FILES ${STRUCT_FILES})
source_group("InstallShield/Wrapper" FILES ${WRAPPER_FILES})

set(CFG_FILES
	src/Decompiler/CFG/BasicBlock.h
    src/Decompiler/CFG/ControlFlowGraph.h
	src/Decompiler/CFG/BasicBlock.cpp
    src/Decompiler/CFG/ControlFlowGraph.cpp
)

set(IRGEN_FILES
	src/Decompiler/IRGen/IRGenerator.h
    src/Decompiler/IRGen/IRStatements.h
	src/Decompiler/IRGen/IRGenerator.cpp
    src/Decompiler/IRGen/IRStatements.cpp
)

set(PARSER_FILES
	src/Decompiler/Parser/Expressions.h
    src/Decompiler/Parser/Parser.h
	src/Decompiler/Parser/Expressions.cpp
    src/Decompiler/Parser/Parser.cpp
)

set(DECOMPILER_FILES
    src/Decompiler/Decompiler.cpp
    src/Decompiler/Function.cpp
    src/Decompiler/GraphVizPrinter.cpp
	src/Decompiler/AbstractVisitor.h
    src/Decompiler/Decompiler.h
    src/Decompiler/Function.h
    src/Decompiler/GraphVizPrinter.h
)

source_group("Decompiler" FILES ${DECOMPILER_FILES})

list(APPEND DECOMPILER_FILES ${CFG_FILES} ${IRGEN_FILES} ${PARSER_FILES})

#add_library(Decompiler ${FILES})

#target_include_directories(Decompiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

source_group("Decompiler/CFG" FILES ${CFG_FILES})
source_group("Decompiler/IRGen" FILES ${IRGEN_FILES})
source_group("Decompiler/Parser" FILES ${PARSER_FILES})

set(ALL_SOURCE_FILES
	${INSTALLSHIELD_FILES}
	${DECOMPILER_FILES}
)

# fetch latest argparse
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

add_executable(InstallScriptDecompiler
    src/main.cpp
	${ALL_SOURCE_FILES}
)

target_link_libraries(InstallScriptDecompiler argparse)

target_include_directories(InstallScriptDecompiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Decompiler ${CMAKE_CURRENT_SOURCE_DIR}/src/InstallShield)

source_group("", FILES src/main.cpp)