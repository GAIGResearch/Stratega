#pragma once
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	class OSLAAgent : public Agent
	{
	public:
		ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, long timeBudgetMs) override;
	};
}