#pragma once

#include "DataDeclList.h"
#include "ScriptFunction.h"
#include "ScriptStruct.h"

class CFrontend
{
public:
	virtual const CDataDeclList& GetGlobalDeclList() const = 0;
	virtual std::vector<ScriptFunction> GetFns() const = 0;
	friend std::ostream& operator<<(std::ostream& out, const CFrontend& o);
	virtual const ScriptStruct* GetStruct(size_t id) const = 0;
	virtual const std::vector<ScriptStruct>& GetStructs() const = 0;
protected:
	virtual void print(std::ostream& os) const = 0;
private:
};