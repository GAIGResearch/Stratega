#pragma once
#include <queue>

namespace SGA
{
	struct GameState;

	struct ActionQueuePack
	{
		enum class ActionSourceType
		{
			Entity,
			Player
		};

		int sourceID;
		int actionTypeID;
		ActionSourceType sourceType;

		ActionQueuePack(ActionSourceType newSourceType, int newSourceID, int newActionTypeID)
		{
			sourceID = newSourceID;
			actionTypeID = newActionTypeID;
			sourceType = newSourceType;
		}
	};

	struct ActionQueue
	{
		//Player ActionQueues
		std::vector<std::queue<ActionQueuePack>> playerActionQueues;

		//Player actions queue
		void addActionToPlayerQueue(ActionQueuePack::ActionSourceType sourceType, int playerID, int sourceID, int newActionTypeID);

		ActionQueuePack getActionFromPlayerQueue(int playerID) const;

		void removeActionFromPlayerQueue(int playerID);

		bool hasActionInPlayerQueue(int playerID) const;

		void validateNextActions(GameState& newState);

		//Initialize queue
		void resizePlayerActionQueues(int playerNum);
	};

}