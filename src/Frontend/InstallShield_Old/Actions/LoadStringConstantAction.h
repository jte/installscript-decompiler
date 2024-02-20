#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

	class CLoadStringConstantAction : public CActionWithArgs
	{
	public:
		CLoadStringConstantAction(CIScript* script, StreamPtr& filePtr);
	protected:
		void print(std::ostream& os) const override;
	private:
	};

};
