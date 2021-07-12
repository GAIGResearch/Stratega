#include <cassert>
#include <Stratega/ForwardModel/Action.h>
#include <Stratega/Representation/GameState.h>

namespace SGA
{
	void Action::execute(GameState& state, const ForwardModel& fm) const
	{
		auto& type = state.getGameInfo()->actionTypes->at(getActionTypeID());
		for (auto& effect : type.effects)
		{
			effect->execute(state,fm, targets);
		}
	}

	bool Action::validate(GameState& state) const
	{
		//Continuous or special action
		if (getActionTypeID()==-1)
			return true;

		auto& actionType = getActionType();
		
		//Check if source and targets are valid
		if(isEntityAction())
		{
			auto entityID = targets.at(0).getEntityID();
			auto* entity=state.getEntity(entityID);
						
			if(entity!=nullptr)
			{
				// Check if this action can be executed		
				if (state.getCurrentTick() - entity->getActionInfo(getActionTypeID()).lastExecutedTick < actionType.cooldownTicks)
					return false;
				
				//Check preconditions
				for (auto& precondition : actionType.preconditions)
				{
					if (!precondition->isFulfilled(state, targets))
						return false;
				}
				
				//Check if action targets are still valid in gamestate
				for (auto& target : targets)
				{
					if (!target.isValid(state))
						return false;
				}
				
				//Check target conditions
				for (auto& actionTarget : actionType.actionTargets)
				{
					for (auto& condition : actionTarget.second)
					{
						if (!condition->isFulfilled(state, targets))
							return false;
					}
				}
				
				return true;
			}

			return false;
			
		}
		else
		{
			// Check if this action can be executed		
			auto* player = state.getPlayer(targets.at(0).getPlayerID());
			if (state.getCurrentTick() - player->getActionInfo(getActionTypeID()).lastExecutedTick < actionType.cooldownTicks)
				return true;

			//Not found
			return false;
		}
	}

	bool Action::isEntityAction() const
	{
		return targets.at(0).getType() == ActionTarget::EntityReference;
	}
	
	bool Action::isPlayerAction() const
	{
		return targets.at(0).getType() == ActionTarget::PlayerReference;
	}

	[[nodiscard]] int Action::getSourceID() const
	{
		return isEntityAction() ? targets.at(0).getEntityID() : targets.at(0).getPlayerID();
	}

	int Action::getActionTypeID() const
	{
		int actionTypeID = -1;
		if (actionType)
			actionTypeID = actionType->id;
		
		return actionTypeID;
	}

	std::string Action::getActionName() const
	{
		std::string name = "undefined";

		if (actionTypeFlags == ActionFlag::EndTickAction)
			name = "End Turn / Pass Action";

		if (actionType)
			name = actionType->name;

		return name;
	}


	const ActionSourceType Action::getActionSourceType() const
	{
		if(actionTypeFlags == ActionFlag::EndTickAction)
			return SGA::ActionSourceType::Player;

		if (actionType)
			return actionType->sourceType;

		return SGA::ActionSourceType::Player;
	}
}
