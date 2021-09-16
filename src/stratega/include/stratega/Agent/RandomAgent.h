#pragma once
#include <stratega/Agent/Agent.h>

namespace SGA
{
	class RandomAgent : public Agent
	{
	public:
		using Agent::Agent;
		ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;
	};
}