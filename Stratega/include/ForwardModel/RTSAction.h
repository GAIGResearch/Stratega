#pragma once
#include <Representation/Vector2.h>
#include <Representation/RTSGameState.h>

namespace SGA
{
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
