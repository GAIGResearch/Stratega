#pragma once
#include <ForwardModel/ForwardModelBase.h>
#include <ForwardModel/Action.h>
#include <ForwardModel/ActionSpaceBase.h>
#include <Representation/GameState.h>
#include <Configuration/WinConditionType.h>
namespace SGA
{
	template <typename GameState=GameState>
	class ForwardModel : public ForwardModelBase<GameState, Action>
	{
	public:
		std::shared_ptr<ActionSpaceBase<GameState>> actionSpace;
		
		WinConditionType winCondition;
		int unitTypeID;
	
		ForwardModel():
			actionSpace(std::make_unique<ActionSpaceBase<GameState>>()),
			winCondition(),
			unitTypeID(0)
		{
		}

		void advanceGameState(GameState& state, const Action& action) const override=0;
	
		std::vector<Action> generateActions(GameState& state) const override
		{
			return actionSpace->generateActions(state,0);
		}
	
		std::vector<Action> generateActions(GameState& state, int playerID) const override
		{
			return actionSpace->generateActions(state,playerID);
		}

		bool isValid(const GameState& state, const Action& action)
		{
			return true;
		}
	
		virtual bool checkGameIsFinished(GameState& state) const = 0;

		// ActionSpaces
		void setActionSpace(std::unique_ptr<ActionSpaceBase<GameState>> as)
		{
			actionSpace = std::move(as);
		}

		ActionSpaceBase<GameState>& getActionSpace() const
		{
			return *actionSpace;
		}

		std::unique_ptr<ActionSpaceBase<GameState>> generateDefaultActionSpace() const
		{
			return std::make_unique<ActionSpaceBase<GameState>>();
		}
	};
}