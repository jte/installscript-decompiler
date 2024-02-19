#pragma once

#include <vector>
#include "Action.h"

class ISBasicBlock
{
public:
	void AddAction(CAction* act)
	{
		m_acts.push_back(act);
	}
	const std::vector<CAction*>& GetActions() const
	{
		return m_acts;
	}
	void SetBBId(uint32_t bbId)
	{
		m_bbId = bbId;
	}
	uint32_t GetBBId() const
	{
		return m_bbId;
	}
private:
	uint32_t m_bbId = 0;
	std::vector<CAction*> m_acts;
};