#pragma once
#include <vector>

namespace SGA
{
	enum class ForwardModelType
	{
		Undefined,
		TBS,
		RTS
	};

	template<typename GameState, typename Action>
	class ForwardModel
	{
	public:
		static const ForwardModelType TYPE = ForwardModelType::Undefined;
		typedef GameState GameStateType;
		typedef Action ActionType;

		void advanceGameState(GameState& state, const Action& action) = delete;
		std::vector<Action> generateActions(const GameState& state) = delete;
	};
}