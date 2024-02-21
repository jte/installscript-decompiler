#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

	class CPlaceholderAction : public CActionWithArgs
	{
	protected:
		void print(std::ostream& os) const override;
	public:
		CPlaceholderAction(CIScript* script, StreamPtr& filePtr);
	};

};