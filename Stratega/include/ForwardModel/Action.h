#pragma once
#include <ForwardModel/ActionType.h>
#include <Representation/Vector2.h>

namespace SGA
{	template<class T>
	class Action
	{
	public:
		Action() = default;
		Action(ActionType type,int playerID,int unitID=-1, T targetPos = T(0, 0), int targetID = -1)
		{
			this->type = type;
			this->playerID = playerID;
			sourceUnitID = unitID;
			targetPosition = targetPos;
			targetUnitID = targetID;
		}

		const ActionType& getType() const
		{
			return type;
		}

		const int& getSourceUnitID() const { return sourceUnitID; }
		const int& getTargetUnitID() const  { return targetUnitID; }
		const T& getTargetPosition() const { return targetPosition; }
		const int getPlayerID() const { return playerID; }

		void updateTargetPosition(T& target) { targetPosition = target; }	

	private:
		ActionType type;

		//IDs
		int targetUnitID;
		int sourceUnitID;

		T targetPosition;

		int playerID;
		
	};
}

