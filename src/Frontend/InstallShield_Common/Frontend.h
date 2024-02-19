#pragma once

#include "DataDeclList.h"
#include "ScriptFunction.h"

class CFrontend
{
public:
	virtual const CDataDeclList& GetGlobalDeclList() const = 0;
	virtual std::vector<ScriptFunction> GetFns() const = 0;
	friend std::ostream& operator<<(std::ostream& out, const CFrontend& o);
protected:
	virtual void print(std::ostream& os) const = 0;
private:
};