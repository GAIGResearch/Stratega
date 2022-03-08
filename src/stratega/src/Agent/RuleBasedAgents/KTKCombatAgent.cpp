#include <Stratega/Agent/RuleBasedAgents/KTKCombatAgent.h>

namespace SGA
{
	std::vector<Action> KTKCombatAgent::filterUnitActions(std::vector<Action>& actions, Entity& unit) const
	{
		std::vector<Action> filteredActions;
		for (const auto& a : actions)
		{
			if (a.getActionTypeID() != -1 && a.isEntityAction() && a.getTargets()[0].getEntityID() == unit.getID())
			{
				filteredActions.emplace_back(a);
			}
		}
		return filteredActions;
	}

	std::vector<Action> KTKCombatAgent::filterActionTypes(std::vector<Action>& actions, std::string type) const
	{
		std::vector<Action> filteredActions;

		for (const auto& a : actions)
		{
			if (actionTypeIDToActionTypeString.at(a.getActionTypeID()) == type)
			{
				filteredActions.emplace_back(a);
			}
		}
		return filteredActions;
	}

	std::vector<Entity> KTKCombatAgent::filterUnitsByReach(const std::vector<Entity>& targetUnits, const Vector2f& pos, GameState& /*currentState*/) const
	{
		std::vector<Entity> units;

		for (const auto& unit : targetUnits)
		{
			double movementRange = unit.getParameter("MovementPoints");
			double attackRange = unit.getParameter("AttackRange");

			if (pos.manhattanDistance(unit.getPosition()) <= attackRange + movementRange)
			{
				units.emplace_back(unit);
			}
		}

		return units;
	}

	std::vector<Action> KTKCombatAgent::filterMovesInRange(const std::vector<SGA::Action>& moves, Vector2f position, int range, GameState& gameState) const
	{
		std::vector<Action> filteredMoves;

		for (const auto& move : moves)
		{
			if (move.getTargets()[1].getPosition(gameState).manhattanDistance(position) <= range)
				filteredMoves.emplace_back(move);
		}

		return filteredMoves;
	}

	int KTKCombatAgent::getPotentialHealing(std::vector<Action>& actions, const Vector2f& pos, const std::vector<Entity>& potentialHealers, GameState& gameState) const
	{
		auto healers = filterUnitsByReach(potentialHealers, pos, gameState);
		auto heal = 0;

		auto healingActions = filterActionTypes(actions, "Heal");

		for (const auto& healer : healers)
		{
			// Check if the unit can heal
			for (auto const& healingAction : healingActions)
			{
				if (healingAction.getTargets()[0].getEntityID() == healer.getID())
				{
					heal += static_cast<int>(healer.getParameter("HealAmount"));
					break;
				}
			}

		}

		return heal;
	}

	bool KTKCombatAgent::getMoveInRange(Entity& u, const Vector2f& pos, int range, const std::vector<Entity>& opponentUnits, std::vector<SGA::Action>& moves, Action& bucket, GameState& gameState) const
	{
		if (u.getPosition().manhattanDistance(pos) <= range)
			return false;

		auto inRangeMoves = filterMovesInRange(moves, pos, range, gameState);
		bool foundAction = false;
		if (inRangeMoves.empty())
		{
			// Move closer to the target position, while keeping a decent strategic position
			double bestScore = std::numeric_limits<double>::lowest();
			// Choose the best strategic position
			for (const auto& move : moves)
			{

				auto dist = pos.manhattanDistance(move.getTargets()[1].getPosition(gameState));
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
				auto score = -getPotentialDamage(move.getTargets()[1].getPosition(gameState), opponentUnits, gameState);
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

	double KTKCombatAgent::getPotentialDamage(const Vector2f& pos, const std::vector<Entity>& potentialAttackers, GameState& gameState) const
	{

		auto attackers = filterUnitsByReach(potentialAttackers, pos, gameState);
		double damage = 0;

		for (const auto& attacker : attackers)
		{
			damage += attacker.getParameter("AttackDamage");
		}

		return damage;
	}

	double KTKCombatAgent::getAttackScore(std::vector<Action>& actions, const Entity& target, const Action& attack, const std::vector<Entity>& opponentUnits, GameState& gameState) const
	{
		Entity* attackTarget = attack.getTargets()[0].getEntity(gameState);
		auto attackAmount = attackTarget->getParameter("AttackDamage");
		auto healAmount = getPotentialHealing(actions, target.getPosition(), opponentUnits, gameState);
		double targetHealth = target.getParameter("Health");

		if (attackAmount >= targetHealth)
		{
			// We can kill the unit immediatly
			return unitScores.at(target.getEntityTypeID()) * 2;
		}
		else if (healAmount < attackAmount)
		{
			// We can kill the unit with an delay
			int turnsToKill = static_cast<int>(std::min(4., std::ceil(targetHealth / (attackAmount - healAmount))));
			return unitScores.at(target.getEntityTypeID()) * (1. + 1. / (1. + turnsToKill));
		}

		// We can't kill the unit alone
		//return unitScores.at(target.typeID) * 0.5;
		return 0;
	}

	double KTKCombatAgent::getHealScore(std::vector<Action>& /*actions*/, const Entity& target, const Action& heal, const std::vector<Entity>& opponentUnits, GameState& gameState) const
	{
		Entity* healTarget = heal.getTargets()[0].getEntity(gameState);
		auto healAmount = healTarget->getParameter("HealAmount");
		double targetHealth = target.getParameter("Health");
		double maxHealth = (gameState.getGameInfo()->getEntityTypes()).at(target.getID()).getParamMax("Health");
		auto resultingHealth = std::min<double>(maxHealth, targetHealth + healAmount);
		auto potentialDamage = getPotentialDamage(target.getPosition(), opponentUnits, gameState);

		if (potentialDamage < 0.0001)
		{
			// The unit is in no immediate danger
			//return unitScores.at(target.typeID) * 0.5 * (resultingHealth - getHealth(&target, gameState)) / static_cast<double>(healAmount);
		}
		else if (potentialDamage > resultingHealth)
		{
			// The unit will die if the opponent wants it
			//return unitScores.at(target.typeID) * 0.2 * (resultingHealth - getHealth(&target, gameSgetMaxHealthtate)) / static_cast<double>(healAmount);
		}
		else if (healAmount >= potentialDamage)
		{
			// We can keep the unit alive forever
			return 2 * unitScores.at(target.getEntityTypeID());
		}
		else
		{
			// We can delay the death
			int turnsUntilDeath = std::min<int>(4, static_cast<int>(std::ceil(targetHealth / (potentialDamage - healAmount))));
			return (1. + turnsUntilDeath / 4.) * unitScores.at(target.getEntityTypeID());
		}
		return 0;
	}

	ActionAssignment KTKCombatAgent::playTurn(GameState& currentState, const ForwardModel& fm)
	{
		//std::cout << "start turn" << std::endl;
		for (const auto& a : currentState.getGameInfo()->getActionTypes())
		{
			actionTypeIDToActionTypeString[a.first] = a.second.getName();
		}
		actionTypeIDToActionTypeString[-1] = "EndTurn";

		auto myUnits = currentState.getPlayerEntities(getPlayerID());
		auto opponentUnits = currentState.getNonPlayerEntities(getPlayerID());


		// Compute the best target that we should attack, based on how much support it has and how easy we can attack it
		Entity* bestAttackTarget = nullptr;
		double highestScore = std::numeric_limits<double>::lowest();
		for (auto opp : opponentUnits)
		{
			// How much support has the unit? Computed by estimating how long it reaches for support to arrive and how strong it is.
			double avgSupportScore = 0;

			std::vector<std::string> eParamNames = opp.getEntityParameterNames();
			std::unordered_map<std::string, double> aParams = opp.getEntityParameters();

			for (auto ally : opponentUnits)
			{
				if (ally.getID() == opp.getID())
					continue;

				int dist = static_cast<int>((opp.getPosition().manhattanDistance(ally.getPosition())));
				int movesToSupport = static_cast<int>(dist / ally.getParameter("MovementPoints"));
				avgSupportScore += unitScores.at(ally.getEntityTypeID()) / (1. + movesToSupport);
			}
			avgSupportScore /= static_cast<double>(opponentUnits.size());


			// How much attack power do we have? Computed by estimating how long it takes to attack and how strong our units are.
			double avgAttackScore = 0;
			for (auto attacker : myUnits)
			{
				int dist = static_cast<int>(opp.getPosition().chebyshevDistance(attacker.getPosition()));
				double movementRange = attacker.getParameter("MovementPoints");
				int movesToAttack = static_cast<int>(std::max(0, dist - static_cast<int>(movementRange)) / movementRange);
				avgAttackScore += unitScores.at(attacker.getEntityTypeID()) / (1. + movesToAttack);
			}
			avgAttackScore /= static_cast<double>(myUnits.size()) + 1;

			// Is this a better target than a previously found target?
			double score = avgAttackScore - avgSupportScore;
			if (score > highestScore)
			{
				highestScore = score;
				bestAttackTarget = &opp;
			}
		}


		Vector2f moveTarget;
		// We found no enemy, so we move to a random position in order to find one
		if (bestAttackTarget == nullptr)
		{
			auto& rngEngine2 = getRNGEngine();
			boost::random::uniform_int_distribution<int> widthDist(0, currentState.getBoardWidth() - 1);
			boost::random::uniform_int_distribution<int> heightDist(0, currentState.getBoardHeight() - 1);
			moveTarget.x = widthDist(rngEngine2);
			moveTarget.y = heightDist(rngEngine2);
		}
		else
		{
			moveTarget = bestAttackTarget->getPosition();
		}

		auto actions = fm.generateActions(currentState, getPlayerID());
		Action nextAction = filterActionTypes(actions, "EndTurn").at(0); // Only one EndTurn action available
		bool foundAction = false;

		for (auto unit : myUnits)
		{
			auto subActions = filterUnitActions(actions, unit);
			// First try attacking something
			highestScore = std::numeric_limits<double>::lowest();
			for (const auto& attack : filterActionTypes(subActions, "Attack"))
			{
				Entity& targetUnit = *attack.getTargets()[1].getEntity(currentState);
				if (targetUnit.getOwnerID() == getPlayerID())
					continue; // No attackerino my own units

				auto score = getAttackScore(actions, targetUnit, attack, opponentUnits, currentState);
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
			for (const auto& heal : filterActionTypes(subActions, "Heal"))
			{
				Entity& targetUnit = *heal.getTargets()[0].getEntity(currentState);
				if (targetUnit.getOwnerID() != getPlayerID())
					continue; // No healerino opponents units

				double targetHealth = targetUnit.getParameter("Health");
				double maxHealth = targetUnit.getEntityType().getParamMax("Health");
				if (targetHealth >= maxHealth)
					continue; // Stop healing units that are already full

				auto score = getHealScore(actions, targetUnit, heal, opponentUnits, currentState);

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
			auto moves = filterActionTypes(subActions, "Move");
			double attackRange = unit.getParameter("AttackRange");
			if (getMoveInRange(unit, moveTarget, static_cast<int>(attackRange), opponentUnits, moves, nextAction, currentState))
			{
				break;
			}
		}
		if (nextAction.getActionFlag() == ActionFlag::EndTickAction)
		{
			//std::cout << "Combat Agent " << "end round" << std::endl;
		}
		else
		{
			//std::cout << "Combat Agent " << "does something" << std::endl;
		}
		//std::cout << "end combat agent" << std::endl;

		return ActionAssignment::fromSingleAction(nextAction);
	}

	void KTKCombatAgent::init(GameState /*initialState*/, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
	{
		//Init stuff goes here.
	}

	ActionAssignment KTKCombatAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer /*timer*/)
	{
		unitScores = UnitTypeEvaluator::getLinearSumEvaluation(state);
		return playTurn(state, forwardModel);
	}
}