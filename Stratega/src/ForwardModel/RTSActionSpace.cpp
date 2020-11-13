#include <ForwardModel/RTSActionSpace.h>

namespace SGA
{
	std::vector<RTSAction> RTSActionSpace::generateActions(RTSGameState& gameState, int playerID)
	{
		std::vector<RTSAction> actionBucket;
		for (auto& unit : gameState.units)
		{
			if (unit.playerID != playerID || unit.intendedAction.type != RTSActionType::None)
				continue;

			generateMoves(unit, actionBucket);
			generateAttacks(unit, actionBucket);
			generateHeals(unit, actionBucket);
		}
		actionBucket.emplace_back(generateEndTickAction());

		return actionBucket;
	}
	
	std::vector<RTSAction> RTSActionSpace::generateActions(RTSGameState& gameState)
	{
		return generateActions(gameState, targetPlayerID);
	}
	
	void RTSActionSpace::generateMoves(RTSUnit& unit, std::vector<RTSAction>& actionBucket) const
	{
		auto& state = unit.state.get();
		int xGrid = static_cast<int>(unit.position.x);
		int yGrid = static_cast<int>(unit.position.y);

		auto startCheckPositionX = std::max(0, xGrid - 1);
		auto endCheckPositionX = std::min(state.board.getWidth() - 1, xGrid + 1);
		auto startCheckPositionY = std::max(0, yGrid - 1);
		auto endCheckPositionY = std::min(state.board.getHeight() - 1, yGrid + 1);

		for (int x = startCheckPositionX; x <= endCheckPositionX; x++)
		{
			for (int y = startCheckPositionY; y <= endCheckPositionY; y++)
			{
				RTSAction action{ RTSActionType::Move, unit.playerID, unit.unitID, SGA::Vector2i(x, y), -1 };
				if (action.validate(state))
					actionBucket.emplace_back(action);
			}
		}
	}
	
	void RTSActionSpace::generateAttacks(RTSUnit& unit, std::vector<RTSAction>& actionBucket) const
	{
		auto& state = unit.state.get();
		for (auto& targetUnit : state.units)
		{
			RTSAction action{ RTSActionType::Attack, unit.playerID, unit.unitID, targetUnit.position, targetUnit.unitID };
			if (action.validate(state))
				actionBucket.emplace_back(action);
		}
	}
	
	void RTSActionSpace::generateHeals(RTSUnit& unit, std::vector<RTSAction>& actionBucket) const
	{
		auto& state = unit.state.get();
		for (auto& targetUnit : state.units)
		{
			RTSAction action{ RTSActionType::Heal, unit.playerID, unit.unitID, targetUnit.position, targetUnit.unitID };
			if (action.validate(state))
				actionBucket.emplace_back(action);
		}
	}

	RTSAction RTSActionSpace::generateEndTickAction() const
	{
		return RTSAction{ RTSActionType::EndTick, -1, -1, {0,0}, -1};
	}
}