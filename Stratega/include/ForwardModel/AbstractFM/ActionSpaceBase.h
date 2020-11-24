#pragma once
#include <ForwardModel/IActionSpace.h>
#include <ForwardModel/AbstractFM/Action.h>
#include <Representation/AbstractGS/GameState.h> // ToDo remove this shiat
#include <Representation/AbstractGS/TBSGameState2.h>

namespace SGA
{
	template<typename GameState = GameState>
	class ActionSpaceBase : public IActionSpace<GameState, Action>
	{
	public:
		std::vector<Action> generateActions(GameState& gameState) override
		{
			for(auto& type : gameState.actionTypes)
			{
			}
		}
	};

	
	class FuckOff : public ActionSpaceBase<TBSGameState2>
	{
	public:
		std::vector<Action> generateActions(TBSGameState2& gameState) override
		{
		}
	};
}