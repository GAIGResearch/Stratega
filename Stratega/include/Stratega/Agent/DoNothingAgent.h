#pragma once
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	class DoNothingAgent : public Agent
	{
	public:
		ActionAssignment computeAction(GameState state, EntityForwardModel& forwardModel, long timeBudgetMs) override;
	};
}