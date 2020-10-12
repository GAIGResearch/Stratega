#include <Agent/CombatAgent.h>

namespace SGA
{
	std::vector<TBSUnit*> CombatAgent::filterUnitsByReach(const std::vector<TBSUnit*>& targetUnits, const Vector2i& pos) const
	{
		std::vector<TBSUnit*> units;
		for (const auto& unit : targetUnits)
		{
			if (pos.manhattanDistance(unit->getPosition()) <= unit->getType().actionRange + unit->getType().movementRange)
			{
				units.emplace_back(unit);
			}
		}

		return units;
	}
	
	std::vector<Action<Vector2i>> CombatAgent::filterMovesInRange(const ActionSpace<Vector2i>& moves, Vector2i position, int range) const
	{
		std::vector<Action<Vector2i>> filteredMoves;
		for (const auto& move : moves)
		{
			if (move.getTargetPosition().manhattanDistance(position) <= range)
				filteredMoves.emplace_back(move);
		}

		return filteredMoves;
	}
	
	int CombatAgent::getPotentialHealing(const Vector2i& pos, const std::vector<TBSUnit*>& potentialHealers) const
	{
		auto healers = filterUnitsByReach(potentialHealers, pos);
		auto heal = 0;
		for (const auto* healer : healers)
		{
			// Check if the unit can heal
			if(healer->getType().actions.find(ActionType::Heal) != healer->getType().actions.end())
				heal += healer->getHealAmount();
		}

		return heal;
	}

	bool CombatAgent::getMoveInRange(TBSUnit& u, const Vector2i& pos, int range, const std::vector<TBSUnit*>& opponentUnits, ActionSpace<Vector2i>& moves, Action<Vector2i>& bucket) const
	{
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
				auto dist = pos.manhattanDistance( move.getTargetPosition());
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
				auto score = -getPotentialDamage(move.getTargetPosition(), opponentUnits);
				if (score > bestScore)
				{
					bestScore = score;
					bucket = move;
					foundAction = true;
				}
			}
		}

		return foundAction;
	}

	double CombatAgent::getPotentialDamage(const Vector2i& pos, const std::vector<TBSUnit*>& potentialAttackers) const
	{
		auto attackers = filterUnitsByReach(potentialAttackers, pos);

		double damage = 0;
		for (const auto* attacker : attackers)
		{
			damage += attacker->getAttackDamage();
		}

		return damage;
	}
	
	double CombatAgent::getAttackScore(const TBSUnit& target, const Action<Vector2i>& attack, const std::vector<TBSUnit*>& opponentUnits) const
	{
		auto& state = target.state.get();
		auto attackAmount = state.getUnit(attack.getSourceUnitID())->getAttackDamage();
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
	}

	double CombatAgent::getHealScore(const TBSUnit& target, const Action<Vector2i>& heal, const std::vector<TBSUnit*>& opponentUnits) const
	{
		auto healAmount = target.state.get().getUnit(heal.getSourceUnitID())->getHealAmount();
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
	}
	
	void CombatAgent::playTurn(TBSGameCommunicator& communicator, TBSForwardModel& fm) const
	{
		while(communicator.isMyTurn() && !communicator.isGameOver())
		{
			auto currentState = communicator.getGameState();

			// Get my units and opponent units
			auto myUnits = currentState.getPlayer(currentState.currentPlayer)->getUnits();
			std::vector<TBSUnit*> opponentUnits;
			for(auto& u : currentState.getUnits())
			{
				if(u.getPlayerID() != communicator.getPlayerID())
				{
					opponentUnits.emplace_back(&u);
				}
			}

			// Compute the best target that we should attack, based on how much support it has and how easy we can attack it
			TBSUnit* bestAttackTarget = nullptr;
			double highestScore = std::numeric_limits<double>::lowest();
			for (auto* opp : opponentUnits)
			{
				// How much support has the unit? Computed by estimating how long it reaches for support to arrive and how strong it is.
				double avgSupportScore = 0;
				for (auto* ally : opponentUnits)
				{
					if (ally->getUnitID() == opp->getUnitID())
						continue;

					int dist = opp->getPosition().manhattanDistance(ally->getPosition());
					int movesToSupport = dist / static_cast<double>(ally->getRange());
					avgSupportScore += unitScores.at(ally->getUnitTypeID()) / (1. + movesToSupport);
				}
				avgSupportScore /= opponentUnits.size();

				// How much attack power do we have? Computed by estimating how long it takes to attack and how strong our units are.
				double avgAttackScore = 0;
				for (auto* attacker : myUnits)
				{
					int dist = opp->getPosition().chebyshevDistance(attacker->getPosition());
					int movesToAttack = std::max(0, dist - attacker->getRange()) / attacker->getRange();
					avgAttackScore += unitScores.at(attacker->getUnitTypeID()) / (1. + movesToAttack);
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

			Vector2i moveTarget;
			// We found no enemy, so we move to a random position in order to find one
			if(bestAttackTarget == nullptr)
			{
				auto& rngEngine = communicator.getRNGEngine();
				std::uniform_int_distribution<int> widthDist(0, currentState.getBoard().getWidth() - 1);
				std::uniform_int_distribution<int> heightDist(0, currentState.getBoard().getHeight() - 1);
				moveTarget.x = widthDist(rngEngine);
				moveTarget.y = heightDist(rngEngine);
			}
			else
			{
				moveTarget = bestAttackTarget->getPosition();
			}

			// Go through all units and return the first action that we deem good
			// Since this function is called multiple times, we will eventually use up all available actions
			auto actions = fm.getActions(currentState);
			Action nextAction = actions->filterActionTypes(ActionType::EndTurn).getAction(0); // Only one EndTurn action available
			bool foundAction = false;
			for(auto* unit : myUnits)
			{
				auto subActions = actions->filterUnitActions(*unit);
				// First try attacking something
				highestScore = std::numeric_limits<double>::lowest();
				for (const auto& attack : subActions.filterActionTypes(ActionType::Attack))
				{
					TBSUnit& targetUnit = *currentState.getUnit(attack.getTargetPosition());
					if (targetUnit.getPlayerID() == communicator.getPlayerID())
						continue; // No attackerino my own units

					auto score = getAttackScore(targetUnit, attack, opponentUnits);
					if (score > highestScore)
					{
						highestScore = score;
						nextAction = attack;
						foundAction = true;
					}
				}

				if (foundAction)
					break;

				// Try healing something
				highestScore = std::numeric_limits<double>::lowest();
				for (const auto& heal : subActions.filterActionTypes(ActionType::Heal))
				{
					TBSUnit& targetUnit = *currentState.getUnit(heal.getSourceUnitID());
					if (targetUnit.getPlayerID() != communicator.getPlayerID())
						continue; // No healerino opponents units
					if (targetUnit.getHealth() >= targetUnit.getType().maxHealth)
						continue; // Stop healing units that are already full, what is wrong with you

					auto score = getHealScore(targetUnit, heal, opponentUnits);
					if (score > highestScore)
					{
						highestScore = score;
						nextAction = heal;
						foundAction = true;
					}
				}

				if (foundAction)
					break;

				// At last, try moving closer to the best attack target
				auto moves = subActions.filterActionTypes(ActionType::Move);
				if (getMoveInRange(*unit, moveTarget, unit->getType().actionRange, opponentUnits, moves, nextAction))
				{
					break;
				}
			}

			communicator.executeAction(nextAction);
		}
	}

	void CombatAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		auto currentState = gameCommunicator.getGameState();
		unitScores = UnitTypeEvaluator::getLinearSumEvaluation(currentState);
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				playTurn(gameCommunicator, forwardModel);
			}
		}
	}

}