##
##

set(ACTIONS_FILES
    Actions/AbortAction.cpp
    Actions/AbortAction.h
    Actions/Action.cpp
    Actions/Action.h
    Actions/ActionWithArgs.cpp
    Actions/ActionWithArgs.h
    Actions/AddressOfAction.cpp
    Actions/AddressOfAction.h
    Actions/AddressOfWideAction.cpp
    Actions/AddressOfWideAction.h
    Actions/AssignAction.cpp
    Actions/AssignAction.h
    Actions/BinaryAction.cpp
    Actions/BinaryAction.h
    Actions/BindVariableAction.cpp
    Actions/BindVariableAction.h
    Actions/DLLFuncCallAction.cpp
    Actions/DLLFuncCallAction.h
    Actions/EndCatchAction.cpp
    Actions/EndCatchAction.h
    Actions/EndFuncAction.cpp
    Actions/EndFuncAction.h
    Actions/EndTryAction.cpp
    Actions/EndTryAction.h
    Actions/ExitAction.cpp
    Actions/ExitAction.h
    Actions/FuncCallAction.cpp
    Actions/FuncCallAction.h
    Actions/FuncPrologAction.cpp
    Actions/FuncPrologAction.h
    Actions/GetByteAction.cpp
    Actions/GetByteAction.h
    Actions/GotoAction.cpp
    Actions/GotoAction.h
    Actions/IfAction.cpp
    Actions/IfAction.h
    Actions/InternalFuncCallAction.cpp
    Actions/InternalFuncCallAction.h
    Actions/NOPAction.cpp
    Actions/NOPAction.h
    Actions/PropGetAction.cpp
    Actions/PropGetAction.h
    Actions/PropPutAction.cpp
    Actions/PropPutAction.h
    Actions/PropPutRefAction.cpp
    Actions/PropPutRefAction.h
    Actions/ReturnAction.cpp
    Actions/ReturnAction.h
    Actions/SetByteAction.cpp
    Actions/SetByteAction.h
    Actions/TryAction.cpp
    Actions/TryAction.h
    Actions/UnaryAction.cpp
    Actions/UnaryAction.h
    Actions/UnUseDLLAction.cpp
    Actions/UnUseDLLAction.h
    Actions/UseDLLAction.cpp
    Actions/UseDLLAction.h
	Actions/StrLengthCharsAction.h
	Actions/StrSubAction.h
	Actions/StrFindAction.h
	Actions/StrCompareAction.h
	Actions/StrToNumAction.h
	Actions/NumToStrAction.h
	Actions/HandlerAction.h
	Actions/HandlerExAction.h
	Actions/DoHandlerAction.h
	Actions/ResizeAction.h
	Actions/SizeofAction.h
	Actions/StrLengthCharsAction.cpp
	Actions/StrSubAction.cpp
	Actions/StrFindAction.cpp
	Actions/StrCompareAction.cpp
	Actions/StrToNumAction.cpp
	Actions/NumToStrAction.cpp
	Actions/HandlerAction.cpp
	Actions/HandlerExAction.cpp
	Actions/DoHandlerAction.cpp
	Actions/ResizeAction.cpp
	Actions/SizeofAction.cpp
	Actions/IndirectStructAction.cpp
	Actions/IndirectStructAction.h
)

set(PROTOTYPE_FILES
    Prototype/DLLPrototype.cpp
    Prototype/DLLPrototype.h
    Prototype/InternalPrototype.cpp
    Prototype/InternalPrototype.h
    Prototype/Prototype.cpp
    Prototype/Prototype.h
)

set(STRUCT_FILES
    Struct/Struct.cpp
    Struct/Struct.h
)

set(WRAPPER_FILES
    Wrapper/LibFile.cpp
    Wrapper/LibFile.h
)

set(INSTALLSHIELD_FILES
    IScript.cpp
    IScript.h
)

source_group("InstallShield" FILES ${INSTALLSHIELD_FILES})

list(APPEND INSTALLSHIELD_FILES ${WRAPPER_FILES} ${STRUCT_FILES} ${PROTOTYPE_FILES} ${ACTIONS_FILES})

source_group("InstallShield/Actions" FILES ${ACTIONS_FILES})
source_group("InstallShield/Prototype" FILES ${PROTOTYPE_FILES})
source_group("InstallShield/Struct" FILES ${STRUCT_FILES})
source_group("InstallShield/Wrapper" FILES ${WRAPPER_FILES})



add_library(InstallShield_New STATIC ${INSTALLSHIELD_FILES})

target_link_libraries(InstallShield_New InstallShield_Common)

target_include_directories(InstallShield_New PRIVATE ${ISIS_BUILD_DIR}/src/Frontend/InstallShield_Common)
target_include_directories(InstallShield_New PRIVATE ${ISIS_BUILD_DIR}/src/Frontend/InstallShield_New)
target_include_directories(InstallShield_New PRIVATE ${ISIS_BUILD_DIR}/src/Decompiler)
target_include_directories(InstallShield_New PRIVATE ${ISIS_BUILD_DIR}/src/Frontend)