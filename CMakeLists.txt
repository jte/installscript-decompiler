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

set(VARIABLES_FILES
	src/Decompiler/Variables/Variable.h
    src/Decompiler/Variables/NumberVariable.h
	src/Decompiler/Variables/StringVariable.h
	src/Decompiler/Variables/VariantVariable.h
	src/Decompiler/Variables/SymbolTable.h
	src/Decompiler/Variables/Variable.cpp
    src/Decompiler/Variables/NumberVariable.cpp
	src/Decompiler/Variables/StringVariable.cpp
	src/Decompiler/Variables/VariantVariable.cpp
	src/Decompiler/Variables/SymbolTable.cpp
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

list(APPEND DECOMPILER_FILES ${CFG_FILES} ${IRGEN_FILES} ${PARSER_FILES} ${VARIABLES_FILES})

source_group("Decompiler/CFG" FILES ${CFG_FILES})
source_group("Decompiler/IRGen" FILES ${IRGEN_FILES})
source_group("Decompiler/Parser" FILES ${PARSER_FILES})
source_group("Decompiler/Variables" FILES ${VARIABLES_FILES})

set(ALL_SOURCE_FILES
	${DECOMPILER_FILES}
)

add_subdirectory(src/Frontend)

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

target_link_libraries(InstallScriptDecompiler argparse InstallShield_Common InstallShield_Old InstallShield_New)

target_include_directories(InstallScriptDecompiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Decompiler ${CMAKE_CURRENT_SOURCE_DIR}/src/Frontend/InstallShield_Common ${ISIS_BUILD_DIR}/src/Frontend)

source_group("", FILES src/main.cpp)