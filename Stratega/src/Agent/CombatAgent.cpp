#include <Stratega/Agent/CombatAgent.h>

namespace SGA
{
	std::vector<Action> CombatAgent::filterUnitActions(std::vector<Action>& actions, Entity& unit) const
	{
		std::vector<Action> filteredActions;
		for (const auto& a : actions)
		{
			if (a.isEntityAction() && a.targets[0].getEntityID() == unit.id)
			{
				
			}
			std::cout << std::endl;
			/*if (a.sourceUnitID == unit.getUnitID())
			{
				filteredActions.emplace_back(a);
			}*/
		}
		return filteredActions;
	}

	std::vector<Action> CombatAgent::filterActionTypes(std::vector<Action>& actions, std::string type) const
	{
		std::vector<Action> filteredActions;

		for (const auto& a : actions)
		{
			if (actionTypeIDToActionTypeString.at(a.actionTypeID) == type)
			{
				filteredActions.emplace_back(a);
			}
		}
		return filteredActions;
	}

	std::vector<Entity*> CombatAgent::filterUnitsByReach(const std::vector<Entity*>& targetUnits, const Vector2i& pos) const
	{
		std::vector<Entity*> units;
		/*
		for (const auto& unit : targetUnits)
		{
			if (pos.manhattanDistance(unit->getPosition()) <= unit->getType().actionRange + unit->getType().movementRange)
			{
				units.emplace_back(unit);
			}
		}
		*/
		return units;
	}

	std::vector<Action> CombatAgent::filterMovesInRange(const std::vector<SGA::Action>& moves, Vector2i position, int range) const
	{
		std::vector<Action> filteredMoves;
		/*
		for (const auto& move : moves)
		{
			if (move.targetPosition.manhattanDistance(position) <= range)
				filteredMoves.emplace_back(move);
		}
		*/
		return filteredMoves;
	}

	int CombatAgent::getPotentialHealing(const Vector2i& pos, const std::vector<Entity*>& potentialHealers) const
	{
		auto healers = filterUnitsByReach(potentialHealers, pos);
		auto heal = 0;
		/*
		for (const auto* healer : healers)
		{
			// Check if the unit can heal
			if (healer->getType().tbsActions.find(ActionType::Heal) != healer->getType().tbsActions.end())
				heal += healer->getHealAmount();
		}
		*/
		return heal;
	}

	bool CombatAgent::getMoveInRange(Entity& u, const Vector2i& pos, int range, const std::vector<Entity*>& opponentUnits, std::vector<SGA::Action>& moves, Action& bucket) const
	{
		return true;
		/*
		if (u.getPosition().manhattanDistance(pos) <= range)
			return false;

		auto inRangeMoves = filterMovesInRange(moves, pos, range);
		bool foundAction = false;
		if (inRangeMoves.empty())
		{
			// Move closer to the target position, while keeping a decent strategic position
			double bestScore = std::numeric_limits<double>::lowest();
			// Choose the best strategic position
			for (const auto& move : moves)
			{
				auto dist = pos.manhattanDistance(move.targetPosition);
				auto score = -dist;
				if (score > bestScore)
				{
					bestScore = score;
					bucket = move;
					foundAction = true;
				}
			}
		}
		else
		{
			double bestScore = std::numeric_limits<double>::lowest();
			// Choose the best strategic position
			for (const auto& move : inRangeMoves)
			{
				auto score = -getPotentialDamage(move.targetPosition, opponentUnits);
				if (score > bestScore)
				{
					bestScore = score;
					bucket = move;
					foundAction = true;
				}
			}
		}

		return foundAction;
		*/
	}

	double CombatAgent::getPotentialDamage(const Vector2i& pos, const std::vector<Entity*>& potentialAttackers) const
	{
		
		auto attackers = filterUnitsByReach(potentialAttackers, pos);
		double damage = 0;
		/*
		for (const auto* attacker : attackers)
		{
			damage += attacker->getAttackDamage();
		}
		*/
		return damage;
	}

	double CombatAgent::getAttackScore(const Entity& target, const Action& attack, const std::vector<Entity*>& opponentUnits) const
	{
		return 0;
		/*
		auto& state = target.state.get();
		auto attackAmount = state.getUnit(attack.sourceUnitID)->getAttackDamage();
		auto healAmount = getPotentialHealing(target.getPosition(), opponentUnits);

		if (attackAmount >= target.getHealthh())
		{
			// We can kill the unit immediatly
			return unitScores.at(target.getUnitTypeID()) * 2;
		}
		else if (healAmount < attackAmount)
		{
			// We can kill the unit with an delay
			int turnsToKill = std::min(4., std::ceil(target.getHealthh() / (attackAmount - healAmount)));
			return unitScores.at(target.getUnitTypeID()) * (1. + 1. / (1. + turnsToKill));
		}

		// We can't kill the unit alone
		return unitScores.at(target.getUnitTypeID()) * 0.5;
		*/
	}

	double CombatAgent::getHealScore(const Entity& target, const Action& heal, const std::vector<Entity*>& opponentUnits) const
	{
		return 0;
		/*
		auto healAmount = target.state.get().getUnit(heal.sourceUnitID)->getHealAmount();
		auto resultingHealth = std::min(target.getType().maxHealth, target.getHealthh() + healAmount);
		auto potentialDamage = getPotentialDamage(target.getPosition(), opponentUnits);

		if (potentialDamage < 0.0001)
		{
			// The unit is in no immediate danger
			return unitScores.at(target.getUnitTypeID()) * 0.5 * (resultingHealth - target.getHealthh()) / static_cast<double>(healAmount);
		}
		else if (potentialDamage > resultingHealth)
		{
			// The unit will die if the opponent wants it
			return unitScores.at(target.getUnitTypeID()) * 0.2 * (resultingHealth - target.getHealthh()) / static_cast<double>(healAmount);
		}
		else if (healAmount >= potentialDamage)
		{
			// We can keep the unit alive forever
			return 2 * unitScores.at(target.getUnitTypeID());
		}
		else
		{
			// We can delay the death
			int turnsUntilDeath = std::min(4, static_cast<int>(std::ceil(target.getHealthh() / (potentialDamage - healAmount))));
			return (1. + turnsUntilDeath / 4.) * unitScores.at(target.getUnitTypeID());
		}
		*/
	}

	double getRange(Entity* entity, TBSGameState& gamestate)
	{
		return 0;
	}
	
	double getHealth(Entity* entity, TBSGameState& gamestate)
	{
		return 0;
	}

	double getMaxHealth(Entity* entity, TBSGameState& gamestate)
	{
		return 0;
	}

	void CombatAgent::playTurn(TBSGameCommunicator& communicator, TBSForwardModel& fm)
	{
		while (communicator.isMyTurn() && !communicator.isGameOver())
		{
			auto currentState = communicator.getGameState();
			for (const auto a : *currentState.actionTypes)
			{
				actionTypeIDToActionTypeString[a.first] = a.second.name;
			}
			actionTypeIDToActionTypeString[-1] = "EndTurn";
			
			currentGameState = &currentState;
			

			//std::cout << "CombatAgent " << currentState.currentGameTurn << std::endl;

			// Get my units and opponent units
			std::vector<Entity*> myUnits;
			std::vector<Entity*> opponentUnits;
			for (auto& entity : currentState.entities) {
				if (entity.ownerID == communicator.getPlayerID())
				{
					myUnits.push_back(&entity);
				}
				else
				{
					opponentUnits.push_back(&entity);
				}
				
			}
			
			
			
			// Compute the best target that we should attack, based on how much support it has and how easy we can attack it
			Entity* bestAttackTarget = nullptr;
			double highestScore = std::numeric_limits<double>::lowest();
			for (auto* opp : opponentUnits)
			{
				
				// How much support has the unit? Computed by estimating how long it reaches for support to arrive and how strong it is.
				double avgSupportScore = 0;
				
				for (auto* ally : opponentUnits)
				{
					if (ally->id == opp->id)
						continue;

					int dist = opp->position.manhattanDistance(ally->position);
					int movesToSupport = dist / static_cast<double>(getRange(ally, currentState));
					//avgSupportScore += unitScores.at(ally->typeID) / (1. + movesToSupport);
				}
				avgSupportScore /= opponentUnits.size();

				
				// How much attack power do we have? Computed by estimating how long it takes to attack and how strong our units are.
				double avgAttackScore = 0;
				for (auto* attacker : myUnits)
				{
					int dist = opp->position.chebyshevDistance(attacker->position);
					int movesToAttack = std::max(0, dist - static_cast<int>(getRange(attacker, currentState))) / getRange(attacker, currentState);
					//avgAttackScore += unitScores.at(attacker->typeID) / (1. + movesToAttack);
				}
				avgAttackScore /= myUnits.size() + 1;

				// Is this a better target than a previously found target?
				double score = avgAttackScore - avgSupportScore;
				if (score > highestScore)
				{
					highestScore = score;
					bestAttackTarget = opp;
				}
				
			}
			
			
			Vector2f moveTarget;
			// We found no enemy, so we move to a random position in order to find one
			if (bestAttackTarget == nullptr)
			{
				auto& rngEngine = communicator.getRNGEngine();
				std::uniform_int_distribution<int> widthDist(0, currentState.board.getWidth() - 1);
				std::uniform_int_distribution<int> heightDist(0, currentState.board.getHeight() - 1);
				moveTarget.x = widthDist(rngEngine);
				moveTarget.y = heightDist(rngEngine);
			}
			else
			{
				moveTarget = bestAttackTarget->position;
			}

			// Go through all units and return the first action that we deem good
			// Since this function is called multiple times, we will eventually use up all available actions

			// delete this
			// until here

			auto actions = fm.generateActions(currentState);
			Action nextAction = filterActionTypes(actions, "EndTurn").at(0); // Only one EndTurn action available
			bool foundAction = false;
			for (auto* unit : myUnits)
			{
				auto subActions = filterUnitActions(actions, *unit);
				// First try attacking something
				highestScore = std::numeric_limits<double>::lowest();
				for (const auto& attack : filterActionTypes(subActions, "Attack"))
				{
					/*Entity& targetUnit = *currentState.getEntity(attack.targetPosition);
					if (targetUnit.ownerID == communicator.getPlayerID())
						continue; // No attackerino my own units

					auto score = getAttackScore(targetUnit, attack, opponentUnits);
					if (score > highestScore)
					{
						highestScore = score;
						nextAction = attack;
						foundAction = true;
					}*/
				}

				if (foundAction)
					break;

				// Try healing something
				highestScore = std::numeric_limits<double>::lowest();
				for (const auto& heal : filterActionTypes(subActions, "Heal"))
				{
					/*Entity& targetUnit = *currentState.getEntity(heal.sourceUnitID);
					if (targetUnit.ownerID != communicator.getPlayerID())
						continue; // No healerino opponents units
					if (getHealth(&targetUnit, currentState) >= getMaxHealth(&targetUnit, currentState))
						continue; // Stop healing units that are already full, what is wrong with you

					auto score = getHealScore(targetUnit, heal, opponentUnits);
					if (score > highestScore)
					{
						highestScore = score;
						nextAction = heal;
						foundAction = true;
					}*/
				}

				if (foundAction)
					break;

				// At last, try moving closer to the best attack target
				auto moves = filterActionTypes(subActions, "Move");
				/*if (getMoveInRange(*unit, moveTarget, unit->getType().actionRange, opponentUnits, moves, nextAction))
				{
					break;
				}*/
			}
			
			communicator.executeAction(nextAction);
			
		}
	}

	void CombatAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		auto currentState = gameCommunicator.getGameState();
		//unitScores = UnitTypeEvaluator::getLinearSumEvaluation(currentState);
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				playTurn(gameCommunicator, forwardModel);
			}
		}
	}

}