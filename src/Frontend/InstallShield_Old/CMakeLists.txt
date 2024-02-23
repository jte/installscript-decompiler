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
	Actions/AssignAction.h
	Actions/AssignAction.cpp
	Actions/BinaryAction.h
	Actions/BinaryAction.cpp
	Actions/IfAction.h
	Actions/IfAction.cpp
	Actions/ReturnFromProgramAction.h
	Actions/ReturnFromProgramAction.cpp
	Actions/UnaryAction.h
	Actions/UnaryAction.cpp
	Actions/NopAction.h
	Actions/NopAction.cpp
	Actions/ReturnAction.h
	Actions/ReturnAction.cpp
	Actions/GotoAction.h
	Actions/GotoAction.cpp
	Actions/LoadStringConstantAction.h
	Actions/LoadStringConstantAction.cpp
	Actions/PropGetAction.h
	Actions/PropGetAction.cpp
	Actions/PropPutAction.h
	Actions/PropPutAction.cpp
	Actions/PlaceholderAction.h
	Actions/PlaceholderAction.cpp
	Actions/EnableAction.h
	Actions/EnableAction.cpp
	Actions/DisableAction.h
	Actions/DisableAction.cpp
	Actions/SetTitleAction.h
	Actions/SetTitleAction.cpp
)

set(INSTALLSHIELD_FILES
    IScript.cpp
    IScript.h
)

source_group("InstallShield" FILES ${INSTALLSHIELD_FILES})

list(APPEND INSTALLSHIELD_FILES ${ACTIONS_FILES})

source_group("InstallShield/Actions" FILES ${ACTIONS_FILES})


add_library(InstallShield_Old STATIC ${INSTALLSHIELD_FILES})

target_link_libraries(InstallShield_Old InstallShield_Common)

target_include_directories(InstallShield_Old PRIVATE ${ISIS_BUILD_DIR}/src/Frontend/InstallShield_Common)
target_include_directories(InstallShield_Old PRIVATE ${ISIS_BUILD_DIR}/src/Frontend/InstallShield_Old)
target_include_directories(InstallShield_Old PRIVATE ${ISIS_BUILD_DIR}/src/Decompiler)
target_include_directories(InstallShield_Old PRIVATE ${ISIS_BUILD_DIR}/src/Frontend)