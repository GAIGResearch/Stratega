#pragma once
#include <vector>

namespace SGA
{
	template<typename GameState, typename Action>
	class IActionSpace
	{
	public:
		virtual ~IActionSpace() = default;
		virtual std::vector<Action> generateActions(GameState& gameState) = 0;
	};
}