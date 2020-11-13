#pragma once
#include <Representation/Vector2.h>
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSActionType.h>

namespace SGA
{	
	class TBSAction
	{
	public:
		TBSAction() :
			type(),
			targetUnitID(-1),
			sourceUnitID(-1),
			targetPosition(),
			playerID(-1)
		{
		}

		TBSAction(TBSActionType type, int playerID, int unitID = -1, Vector2i targetPos = {0, 0}, int targetID = -1)
		{
			this->type = type;
			this->playerID = playerID;
			sourceUnitID = unitID;
			targetPosition = targetPos;
			targetUnitID = targetID;
		}
		
		TBSActionType type;
		int targetUnitID;
		int sourceUnitID;
		Vector2i targetPosition;
		int playerID;

		bool validate(TBSGameState& state) const;
		
	private:
		bool validateMove(TBSGameState& state) const;
		bool validateAttack(TBSGameState& state) const;
		bool validatePush(TBSGameState& state) const;
		bool validateHeal(TBSGameState& state) const;
		bool validateEndOfTurn(TBSGameState& state) const;

	};

	static std::string getActionType(const TBSActionType& action)
	{
		switch (action)
		{
		case TBSActionType::Attack: return "Attack";
		case TBSActionType::Move: return "Move";
		case TBSActionType::Heal: return "Heal";
		case TBSActionType::Push: return "Push";
		case TBSActionType::EndTurn: return "EndTurn";
		default: return "Undefined";
		}
	}
}