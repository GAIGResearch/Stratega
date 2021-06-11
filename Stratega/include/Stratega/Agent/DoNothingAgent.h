#pragma once
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	class DoNothingAgent : public Agent
	{
	public:
		ActionAssignment computeAction(GameState state, const EntityForwardModel* forwardModel, long timeBudgetMs) override;
	};
}