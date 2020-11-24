#pragma once
#include <ForwardModel/IActionSpace.h>
#include <ForwardModel/AbstractFM/Action.h>
//#include <Representation/AbstractGS/GameState.h> // ToDo remove this shiat
#include <Representation/AbstractGS/TBSGameState2.h>

namespace SGA
{
	template<typename GameState=TBSGameState2>
	class ActionSpaceBase : public IActionSpace<GameState, Action>
	{
	public:
		std::vector<Action> generateActions(GameState& gameState) override
		{
			std::vector<Action> temp;
			for(auto& type : *gameState.actionTypes)
			{
				type
			}
			return temp;
		}
	};
}