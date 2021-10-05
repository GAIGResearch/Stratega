#include <Stratega/ForwardModel/ActionAssignment.h>

namespace SGA
{
	void ActionAssignment::assignActionOrReplace(Action newAction)
	{
		if (newAction.isEntityAction())
		{
			auto id = newAction.getSourceID();
			entityActions.emplace(id, std::move(newAction));
		}
		else if (newAction.isPlayerAction())
		{
			auto id = newAction.getSourceID();
			playerActions.emplace(id, std::move(newAction));
		}
		else
		{
			throw std::runtime_error("Tried assigning an unknown action-type to ActionAssignment");
		}
	}

	void ActionAssignment::merge(const ActionAssignment& action)
	{
		for (const auto& idActionPair : action.entityActions)
		{
			entityActions.emplace(idActionPair.first, idActionPair.second);
		}
		for (const auto& idActionPair : action.playerActions)
		{
			playerActions.emplace(idActionPair.first, idActionPair.second);
		}
	}

	void ActionAssignment::clear()
	{
		entityActions.clear();
		playerActions.clear();
	}

	Action* ActionAssignment::getEntityAction(int entityID)
	{
		auto it = entityActions.find(entityID);
		return it == entityActions.end() ? nullptr : &it->second;
	}

	Action* ActionAssignment::getPlayerAction(int playerID)
	{
		auto it = playerActions.find(playerID);
		return it == playerActions.end() ? nullptr : &it->second;
	}

	size_t ActionAssignment::getAssignmentCount() const
	{
		return entityActions.size() + playerActions.size();
	}
	
	const std::unordered_map<int, Action>& ActionAssignment::getEntityActions() const
	{
		return entityActions;
	}

	const std::unordered_map<int, Action>& ActionAssignment::getPlayerActions() const
	{
		return playerActions;
	}

	ActionAssignment ActionAssignment::fromSingleAction(Action a)
	{
		ActionAssignment assignment;
		assignment.assignActionOrReplace(a);
		return assignment;
	}

	ActionAssignment ActionAssignment::createEndActionAssignment(int playerID)
	{
		return fromSingleAction(Action::createEndAction(playerID));
	}

}