#include <ForwardModel/RTSForwardModel.h>

namespace SGA
{
	void RTSForwardModel::advanceGameState(RTSGameState& state, const Action<Vector2f>& action) const
	{
		std::vector<Action<Vector2f>> actions = { action };
		advanceGameState(state, actions);
	}

	void RTSForwardModel::advanceGameState(RTSGameState& state, const std::vector<Action<Vector2f>>& actions) const
	{
		for(const auto& action : actions)
		{
			auto* unit = state.getUnit(action.getSourceUnitID());
			unit->executingAction = std::make_shared<Action<Vector2f>>(action);
		}

		// Execute actions, note that we first execute one ActionType after the other
		// This is to prevent disadvantages in chasing situations for example
		RTSFMState fmState(state);
		for(auto actionType : {ActionType::Move, ActionType::Heal, ActionType::Attack})
		{
			for(auto& unit : state.units)
			{
				if (unit.executingAction == nullptr || unit.executingAction->getType() != actionType)
					continue;

				switch (actionType)
				{
					case ActionType::Move: executeMove(fmState, unit); break;
					case ActionType::Attack: executeAttack(fmState, unit); break;
					case ActionType::Heal: executeHeal(fmState, unit); break;
				}
			}

			if (actionType == ActionType::Move)
			{
				resolveUnitCollisions(fmState);
				resolveEnvironmentCollisions(fmState);
			}
		}
		
		// Remove all units that were killed
		for (auto id : fmState.deadUnitIDs)
		{
			state.removeUnit(id);
		}
		
		// Update cooldown
		for(auto& unit : state.units)
		{
			unit.actionCooldown = std::max(0., unit.actionCooldown - deltaTime);
		}
	}
	
	ActionSpace<Vector2f>* RTSForwardModel::generateActions(RTSGameState& state) const
	{
		throw std::runtime_error("Can't generate actions without an playerID for RTS-Games");
	}

	ActionSpace<Vector2f>* RTSForwardModel::generateActions(RTSGameState& state, int playerID) const
	{
		auto* actionSpace = new ActionSpace<Vector2f>();
		for (auto& unit : state.units)
		{
			if (unit.playerID != playerID)
				continue;
			
			generateMoves(unit, *actionSpace);
			generateAttacks(unit, *actionSpace);
			generateHeals(unit, *actionSpace);
		}

		return actionSpace;
	}

	void RTSForwardModel::generateMoves(RTSUnit& unit, ActionSpace<Vector2f>& actionBucket) const
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
				Action<Vector2f> action{ SGA::ActionType::Move, unit.playerID, unit.unitID, SGA::Vector2i(x, y), -1 };
				if(validateMove(state, action))
					actionBucket.addAction(action);
			}
		}
	}
	
	void RTSForwardModel::generateAttacks(RTSUnit& unit, ActionSpace<Vector2f>& actionBucket) const
	{
		auto& state = unit.state.get();
		for(auto& targetUnit : state.units)
		{
			Action<Vector2f> action{ SGA::ActionType::Attack, unit.playerID, unit.unitID, targetUnit.position, targetUnit.unitID};
			if (validateAttack(state, action))
				actionBucket.addAction(action);
		}
	}
	
	void RTSForwardModel::generateHeals(RTSUnit& unit, ActionSpace<Vector2f>& actionBucket) const
	{
		auto& state = unit.state.get();
		for (auto& targetUnit : state.units)
		{
			Action<Vector2f> action{ SGA::ActionType::Heal, unit.playerID, unit.unitID, targetUnit.position, targetUnit.unitID };
			if (validateHeal(state, action))
				actionBucket.addAction(action);
		}
	}

	bool RTSForwardModel::validateMove(RTSGameState& state, const Action<Vector2f>& action) const
	{
		return state.getUnit(action.getSourceUnitID()) != nullptr;
	}
	
	bool RTSForwardModel::validateAttack(RTSGameState& state, const Action<Vector2f>& action) const
	{
		auto* attacker = state.getUnit(action.getSourceUnitID());
		auto* target = state.getUnit(action.getTargetUnitID());
		if (attacker == nullptr || target == nullptr)
			return false;

		return attacker->unitID != target->unitID && attacker->actionCooldown <= 0 && attacker->position.distance(target->position) <= attacker->actionRange;
	}
	
	bool RTSForwardModel::validateHeal(RTSGameState& state, const Action<Vector2f>& action) const
	{
		auto* healer = state.getUnit(action.getSourceUnitID());
		auto* target = state.getUnit(action.getTargetUnitID());
		if (healer == nullptr || target == nullptr)
			return false;

		return healer->actionCooldown <= 0 && healer->position.distance(target->position) <= healer->actionRange;
	}

	void RTSForwardModel::executeMove(RTSFMState& state, RTSUnit& unit) const
	{
		if (!validateMove(state.target, *unit.executingAction))
		{
			unit.executingAction = nullptr;
			return;
		}
		
		auto movementDir = unit.executingAction->getTargetPosition() - unit.position;
		auto movementDistance = movementDir.magnitude();
		auto movementSpeed = unit.movementSpeed * deltaTime;
		if (movementDistance <= movementSpeed)
		{
			unit.position = unit.executingAction->getTargetPosition();
			unit.executingAction = nullptr;
		}
		else
		{
			unit.position = unit.position + (movementDir / movementDir.magnitude()) * movementSpeed;
		}
	}
	
	void RTSForwardModel::executeAttack(RTSFMState& state, RTSUnit& unit) const
	{
		if (!validateAttack(state.target, *unit.executingAction))
		{
			unit.executingAction = nullptr;
			return;
		}
		
		auto* targetUnit = unit.state.get().getUnit(unit.executingAction->getTargetUnitID());
		targetUnit->health -= unit.attackDamage;
		if (targetUnit->health <= 0)
		{
			state.deadUnitIDs.emplace_back(targetUnit->unitID);
			unit.executingAction = nullptr;
		}

		unit.actionCooldown = unit.maxActionCooldown;
	}
	
	void RTSForwardModel::executeHeal(RTSFMState& state, RTSUnit& unit) const
	{
		if (!validateHeal(state.target, *unit.executingAction))
		{
			unit.executingAction = nullptr;
			return;
		}
		
		auto* targetUnit = unit.state.get().getUnit(unit.executingAction->getTargetUnitID());
		targetUnit->health += unit.healAmount;
		
		unit.actionCooldown = unit.maxActionCooldown;
	}

	void RTSForwardModel::resolveUnitCollisions(RTSFMState& state) const
	{
		for (auto& unit : state.target.units)
		{
			Vector2f pushDir;
			for (auto& otherUnit : state.target.units)
			{
				// Units cant collide with themselves, also idle units do not push busy units
				if (unit.unitID == otherUnit.unitID || (unit.executingAction != nullptr && otherUnit.executingAction == nullptr))
					continue;

				auto dir = otherUnit.position - unit.position;
				if (dir.magnitude() <= unit.collisionRadius + otherUnit.collisionRadius)
				{
					auto penetrationDepth = unit.collisionRadius + otherUnit.collisionRadius - dir.magnitude();
					pushDir = pushDir + dir.normalized() * 2 / (1 + penetrationDepth);
				}
			}

			unit.position = unit.position - pushDir * deltaTime;
		}
	}
	
	void RTSForwardModel::resolveEnvironmentCollisions(RTSFMState& state) const
	{
		static float RECT_SIZE = 1;
		
		// Collision
		for (auto& unit : state.target.units)
		{
			int startCheckPositionX = std::floor(unit.position.x - unit.collisionRadius - RECT_SIZE);
			int endCheckPositionX = std::ceil(unit.position.x + unit.collisionRadius + RECT_SIZE);
			int startCheckPositionY = std::floor(unit.position.y - unit.collisionRadius - RECT_SIZE);
			int endCheckPositionY = std::ceil(unit.position.y + unit.collisionRadius + RECT_SIZE);

			Vector2f pushDir;
			for (int x = startCheckPositionX; x <= endCheckPositionX; x++)
			{
				for (int y = startCheckPositionY; y <= endCheckPositionY; y++)
				{
					// Everything outside bounds is considered as un-walkable tiles
					if (state.target.isInBounds({ x, y }) && state.target.board.getTile(x, y).isWalkable)
						continue;

					// https://stackoverflow.com/questions/45370692/circle-rectangle-collision-response
					auto fx = static_cast<float>(x);
					auto fy = static_cast<float>(y);
					auto nearestX = std::max(fx, std::min(unit.position.x, fx + RECT_SIZE));
					auto nearestY = std::max(fy, std::min(unit.position.y, fy + RECT_SIZE));
					auto dist = unit.position - Vector2f(nearestX, nearestY);
					
					auto penetrationDepth = unit.collisionRadius - dist.magnitude();
					// No collision detected
					if (penetrationDepth <= 0)
						continue;
					
					auto dir = dist.normalized() * penetrationDepth;
					pushDir = pushDir + dir;
				}
			}

			unit.position = unit.position + pushDir;
		}
	}
}