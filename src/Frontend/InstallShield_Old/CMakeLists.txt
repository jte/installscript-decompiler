##
##

set (ACTIONS_FILES
	Actions/Action.h
	Actions/Action.cpp
	Actions/ActionWithArgs.h
	Actions/ActionWithArgs.cpp
	Actions/FuncPrologAction.h
	Actions/FuncPrologAction.cpp
	Actions/InternalFuncCallAction.h
	Actions/InternalFuncCallAction.cpp
	Actions/ReturnEmptyAction.h
	Actions/ReturnEmptyAction.cpp
)

set(INSTALLSHIELD_FILES
    IScript.cpp
    IScript.h
)

source_group("InstallShield" FILES ${INSTALLSHIELD_FILES})

list(APPEND INSTALLSHIELD_FILES ${ACTIONS_FILES})

source_group("InstallShield/Actions" FILES ${ACTIONS_FILES})


add_library(InstallShield_Old STATIC ${INSTALLSHIELD_FILES})

target_include_directories(InstallShield_Old PRIVATE ${ISIS_BUILD_DIR}/src/Frontend/InstallShield_Common)
target_include_directories(InstallShield_Old PRIVATE ${ISIS_BUILD_DIR}/src/Frontend/InstallShield_Old)
target_include_directories(InstallShield_Old PRIVATE ${ISIS_BUILD_DIR}/src/Decompiler)
target_include_directories(InstallShield_Old PRIVATE ${ISIS_BUILD_DIR}/src/Frontend)