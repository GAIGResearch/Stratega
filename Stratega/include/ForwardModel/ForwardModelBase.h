#pragma once
#include <vector>
#include <ForwardModel/ActionSpace.h>

namespace SGA
{
	template<typename State, typename Action>
	class ForwardModelBase
	{
	public:
		virtual ~ForwardModelBase() = default;
		
		virtual void advanceGameState(State& state, const Action& action) const = 0;
		
		virtual std::vector<Action> getActions(State& state) const = 0;
		virtual std::vector<Action> getActions(State& state, int playerID) const = 0;
	};
}