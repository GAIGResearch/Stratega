#pragma once
#include <Representation/GameState.h>
// ToDo Why is this in configuration?
#include <Configuration/WinConditionType.h>

#include <ForwardModel/ActionSpaceBase.h>

namespace SGA
{
	class EntityForwardModel
	{
	public:
		std::shared_ptr<ActionSpaceBase<GameState>> actionSpace;
		std::vector<std::shared_ptr<Effect>> onTickEffects;
		WinConditionType winCondition;
		int targetUnitTypeID;

		virtual ~EntityForwardModel() = default;
		EntityForwardModel()
			: winCondition(WinConditionType::LastManStanding),
			  targetUnitTypeID(-1)
		{
		}
		
		bool canPlayerPlay(Player& player) const;
		void executeAction(GameState& state, const Action& action) const;
		void endTick(GameState& state) const;

		std::unique_ptr<ActionSpaceBase<GameState>> generateDefaultActionSpace()
		{
			return std::make_unique<ActionSpaceBase<GameState>>();
		}
		
		void setActionSpace(std::unique_ptr<ActionSpaceBase<GameState>> actionSpace)
		{
			this->actionSpace = std::move(actionSpace);
		}
	};

}
