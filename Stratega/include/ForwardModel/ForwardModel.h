#pragma once
#include <ForwardModel/ForwardModelBase.h>
#include <ForwardModel/Action.h>
#include <ForwardModel/ActionSpaceBase.h>
#include <Representation/GameState.h>
#include <Configuration/WinConditionType.h>
namespace SGA
{	template <typename Gamestate=GameState>
	class ForwardModel : public ForwardModelBase<Gamestate, Action>
	{
	public:
	std::shared_ptr<ActionSpaceBase<Gamestate>> actionSpace;
	protected:
		
	public:
		WinConditionType winCondition;
		int unitTypeID;
	
		ForwardModel():
		actionSpace(std::make_unique<ActionSpaceBase<Gamestate>>())
		{
			
		}
	
		void advanceGameState(Gamestate& state, const Action& action) const override=0;
	
		std::vector<Action> generateActions(Gamestate& state) const override
		{
			return actionSpace->generateActions(state,0);
		}
	
		std::vector<Action> generateActions(Gamestate& state, int playerID) const override
		{
			return actionSpace->generateActions(state,playerID);
		}

		bool isValid(const Gamestate& state, const Action& action)
		{
			return true;
		}
	
		virtual bool checkGameIsFinished(Gamestate& state) const = 0;

		// ActionSpaces
		void setActionSpace(std::unique_ptr<ActionSpaceBase<Gamestate>> as)
		{
			actionSpace = std::move(as);
		}

		ActionSpaceBase<Gamestate>& getActionSpace() const
		{
			return *actionSpace;
		}

		std::unique_ptr<ActionSpaceBase<Gamestate>> generateDefaultActionSpace() const
		{
			return std::make_unique<ActionSpaceBase<Gamestate>>();
		}
	};
}