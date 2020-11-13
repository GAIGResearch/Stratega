#pragma once
#include <vector>

namespace SGA
{
	template<typename State, typename Action>
	class ForwardModelBase
	{
	public:
		virtual ~ForwardModelBase() = default;
		
		virtual void advanceGameState(State& state, const Action& action) const = 0;
		
		virtual std::vector<Action> generateActions(State& state) const = 0;
		virtual std::vector<Action> generateActions(State& state, int playerID) const = 0;
	};
}