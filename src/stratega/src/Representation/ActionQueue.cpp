#include <Stratega/Representation/ActionQueue.h>
#include <Stratega/Representation/GameState.h>

namespace SGA
{

	void ActionQueue::addActionToPlayerQueue(ActionQueuePack::ActionSourceType sourceType, int playerID, int sourceID, int newActionTypeID)
	{

		playerActionQueues[playerID].push({ sourceType, sourceID, newActionTypeID });
	}

	ActionQueuePack ActionQueue::getActionFromPlayerQueue(int playerID) const
	{
		auto actionPack = playerActionQueues[playerID].front();
		return actionPack;
	}

	void ActionQueue::removeActionFromPlayerQueue(int playerID)
	{
		playerActionQueues[playerID].pop();
	}

	bool ActionQueue::hasActionInPlayerQueue(int playerID) const
	{
		return playerActionQueues[playerID].size() > 0;
	}

	void ActionQueue::validateNextActions(GameState& newState)
	{		
		//GameState& state = const_cast<GameState&>(newState);
		//Check if entity are still valids and remove invalid actions
		int index = 0;
		for (auto& queue : playerActionQueues)
		{
			if (hasActionInPlayerQueue(index))
			{
				if (getActionFromPlayerQueue(index).sourceType == ActionQueuePack::ActionSourceType::Entity)
					if (newState.getEntity(getActionFromPlayerQueue(index).sourceID) == nullptr)
						removeActionFromPlayerQueue(index);
			}
			index++;
		}
	}

	void ActionQueue::resizePlayerActionQueues(int playerNum)
	{
		playerActionQueues.resize(playerNum);
	}
}