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
		virtual void advanceGameState(State& state, const std::vector<Action>& action) const = 0;
		std::unique_ptr<ActionSpaceBase<Action>> getActions(State& state) const
		{
			return std::unique_ptr<ActionSpaceBase<Action>>(generateActions(state));
		}
		
		std::unique_ptr<ActionSpaceBase<Action>> getActions(State& state, int playerID) const
		{
			return std::unique_ptr<ActionSpaceBase<Action>>(generateActions(state, playerID));
		}
		
	private:
		virtual ActionSpaceBase<Action>* generateActions(State& state) const = 0;
		virtual ActionSpaceBase<Action>* generateActions(State& state, int playerID) const = 0;
	};
}