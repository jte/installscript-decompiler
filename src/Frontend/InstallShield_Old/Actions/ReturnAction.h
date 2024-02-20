#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

	class CReturnAction : public CActionWithArgs
	{
	public:
		CReturnAction(CIScript* script, StreamPtr& filePtr);
	protected:
		void print(std::ostream& os) const override;
	private:
	};

};
