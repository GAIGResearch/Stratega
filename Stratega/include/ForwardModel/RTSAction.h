#pragma once
#include <Representation/Vector2.h>

namespace SGA
{
	class RTSGameState;
	
	enum class RTSActionType
	{
		None,
		Move,
		Attack,
		Heal,
		EndTick
	};
	
	class RTSAction
	{
	public:
		RTSAction() :
			type(RTSActionType::None),
			targetUnitID(-1),
			sourceUnitID(-1),
			targetPosition(),
			playerID(-1)
		{
		}

		RTSAction(RTSActionType type, int playerID, int unitID = -1, Vector2f targetPos = { 0, 0 }, int targetID = -1)
		{
			this->type = type;
			this->playerID = playerID;
			sourceUnitID = unitID;
			targetPosition = targetPos;
			targetUnitID = targetID;
		}
		
		RTSActionType type;
		int targetUnitID;
		int sourceUnitID;
		Vector2f targetPosition;
		int playerID;

		bool validate(RTSGameState& state) const;

	private:
		bool validateMove(RTSGameState& state) const;
		bool validateAttack(RTSGameState& state) const;
		bool validateHeal(RTSGameState& state) const;

	};
}
