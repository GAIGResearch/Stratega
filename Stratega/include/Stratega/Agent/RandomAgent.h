#pragma once
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	class RandomAgent : public Agent
	{
	public:
		void init(GameState state, const EntityForwardModel& forwardModel, long timeBudgetMs) override;
		ActionAssignment computeAction(GameState state, const EntityForwardModel& forwardModel, long timeBudgetMs) override;
	};
}
