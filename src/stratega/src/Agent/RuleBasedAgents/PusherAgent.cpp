#include <Stratega/Agent/RuleBasedAgents/PusherAgent.h>

namespace SGA
{
	Direction GetDirectionTo(const Vector2i& from, const Vector2i& to)
	{
		int dx = to.x - from.x;
		int dy = to.y - from.y;

		if (dx > 0)
		{
			if (dy == 0)
				return Direction::East;
			if (dy > 0)
				return Direction::SouthEast;

			return Direction::NorthEast;
		}
		if (dx < 0)
		{
			if (dy == 0)
				return Direction::West;
			if (dy > 0)
				return Direction::SouthWest;

			return Direction::NorthWest;
		}

		if (dy > 0)
			return Direction::South;
		if (dy < 0)
			return Direction::North;

		return Direction::None;
	}

	Direction ReverseDir(const Direction d)
	{
		switch (d)
		{
		case Direction::North: return Direction::South;
		case Direction::NorthEast: return Direction::SouthWest;
		case Direction::East: return Direction::West;
		case Direction::SouthEast: return Direction::NorthWest;
		case Direction::South: return Direction::North;
		case Direction::SouthWest: return Direction::NorthEast;
		case Direction::West: return Direction::East;
		case Direction::NorthWest: return Direction::SouthEast;
		case Direction::None: return Direction::None;
		}

		throw std::runtime_error("Tried reversing an unknown direction.");
	}

	Vector2i MoveTo(const Vector2i& pos, const Direction d)
	{
		switch (d)
		{
		case Direction::North: return Vector2i(pos.x, pos.y - 1);
		case Direction::NorthEast: return Vector2i(pos.x + 1, pos.y - 1);
		case Direction::East: return Vector2i(pos.x + 1, pos.y);
		case Direction::SouthEast: return Vector2i(pos.x + 1, pos.y + 1);
		case Direction::South:  return Vector2i(pos.x, pos.y + 1);
		case Direction::SouthWest: return Vector2i(pos.x - 1, pos.y + 1);
		case Direction::West: return Vector2i(pos.x - 1, pos.y);
		case Direction::NorthWest: return Vector2i(pos.x - 1, pos.y - 1);
		case Direction::None: return pos;
		}

		throw std::runtime_error("Tried moving into an unknown direction.");
	}

	std::vector<Action> PusherAgent::filterUnitActions(std::vector<Action>& actions, Entity& unit) const
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

	std::vector<Action> PusherAgent::filterActionTypes(std::vector<Action>& actions, std::string type) const
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

	void PusherAgent::init(GameState /*initialState*/, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
	{
		//Init stuff goes here.
	}

	ActionAssignment PusherAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer /*timer*/)
	{
		/*unitScores = UnitTypeEvaluator::getLinearSumEvaluation(state);
		return playTurn(state, forwardModel);*/
		return playTurn(state, forwardModel);
	}

	ActionAssignment PusherAgent::playTurn(GameState& newState, const ForwardModel& forwardModel)
	{
		for (const auto& a : newState.getGameInfo()->getActionTypes())
		{
			actionTypeIDToActionTypeString[a.first] = a.second.getName();
		}
		actionTypeIDToActionTypeString[-1] = "EndTurn";

		auto currentState = newState;
		std::vector<Action> actionBucket;

		auto state = newState;

		// Collect units that we posess and opponent units
		std::vector<Entity> myUnits;
		std::vector<Entity> opponentUnits;
		for (auto& unit : state.getEntities())
		{
			if (unit.getOwnerID() == this->getPlayerID())
			{
				myUnits.push_back(unit);
			}
			else
			{
				opponentUnits.push_back(unit);
			}
		}

		// Go through all units and return the first action that we deem good
		// Since this function is called multiple times, we will eventually use up all actions available
		//auto bestAction = forwardModel.actionSpace->generateEndOfTurnAction(state, gameCommunicator.getPlayerID());
		auto actions = forwardModel.generateActions(currentState, getPlayerID());
		Action bestAction = filterActionTypes(actions, "EndTurn").at(0); // Only one EndTurn action available

		for (auto& u : myUnits)
		{
			std::vector<Vector2i> possibleTargets;
			std::vector<Entity> attackTarget;
			std::vector<int> pushCount;
			// Compute all positions where we could attack an unit and store it in possibleTargets
			for (auto& opponent : opponentUnits)
			{
				Vector2i opponentPositon = Vector2i(opponent.getPosition().x, opponent.getPosition().y);
				auto attacks = getAttackDirections(state, opponentPositon);
				for (auto& attack : attacks)
				{
					auto target = MoveTo(opponentPositon, attack.first);
					if (state.isWalkable(target))
					{
						possibleTargets.emplace_back(target);
						pushCount.emplace_back(attack.second);
						attackTarget.emplace_back(opponent);
					}
				}
			}

			auto paths = ShortestPaths(state, { static_cast<int>(u.getPosition().x),static_cast<int>(u.getPosition().y) }, possibleTargets);
			int bestAttackIndex = -1;
			double lowestCost = std::numeric_limits<double>::max();
			// Search for the best attack target by using an heuristic to estimate how good one attack is
			for (auto i = 0; i < paths.size(); i++)
			{
				double costOutput = 0;
				// Is this a better attack than a previously found attack?
				auto isSafe = analyzePath(state, attackTarget[i], opponentUnits, paths[i].second, pushCount[i], costOutput);
				if (paths[i].first && isSafe && costOutput < lowestCost)
				{
					lowestCost = costOutput;
					bestAttackIndex = i;
				}
			}

			// After finding our attack target, first try to push an opponent. But only if we can't attack our attack target immediately
			if (bestAttackIndex == -1 || paths[bestAttackIndex].second.size() != 1)
			{
				actionBucket.clear();
				std::vector<Action> pushActions = filterActionTypes(actions, "Push");
				actionBucket.insert(actionBucket.end(), pushActions.begin(), pushActions.end());
				//actionBucket.emplace_back(pushActions);
				//forwardModel.actionSpace->generatePushActions(u, actionBucket);

				int minimumPushCount = std::numeric_limits<int>::max();
				for (auto& push : actionBucket)
				{
					auto* pushTarget = push.getTargets()[1].getEntity(state);
					//auto* pushTarget = state.getEntityAt(push.getTargets()[1].getEntity());
					if (pushTarget->getOwnerID() == this->getPlayerID())
						continue; // Do not push our own units

					// We can attack our attack target immediately, so do that
					if (bestAttackIndex != -1 && pushTarget->getID() == attackTarget[bestAttackIndex].getID() && paths[bestAttackIndex].second.empty())
					{
						bestAction = push;
						break;
					}

					// Compute the attack direction from the perspective of the opponent
					auto pos = push.getTargets()[1].getPosition(state);
					Vector2i targetPosition = { static_cast<int>(pos.x), static_cast<int>(pos.y) };
					auto pushDir = GetDirectionTo(targetPosition, { static_cast<int>(u.getPosition().x), static_cast<int>(u.getPosition().y) });
					//auto pushDir = GetDirectionTo(push.targetPosition, { u.getPosition().x,u.getPosition().y });
					// How good is this attack?
					auto attacks = getAttackDirections(state, targetPosition);
					if (attacks[pushDir] < minimumPushCount)
					{
						bestAction = push;
						minimumPushCount = attacks[pushDir];
					}
				}
			}

			if (bestAction.getActionFlag() != ActionFlag::EndTickAction)
				break;
			
			if (bestAttackIndex != -1)
			{
				// We found an path to attack an opponent, now find an available move that follows our computed path
				auto& path = paths[bestAttackIndex].second;
				std::unordered_set<Vector2i, VectorHash2i> pathTilesLookup;
				for (auto entry : path)
				{
					pathTilesLookup.insert(entry);
				}

				actionBucket.clear();
				auto moveActions = filterActionTypes(actions, "Move");
				actionBucket.insert(actionBucket.end(), moveActions.begin(), moveActions.end());
				//actionBucket.emplace_back(moveActions);
				//forwardModel.actionSpace->generateMoveActions(u, actionBucket);

				for (auto& move : actionBucket)
				{
					auto pos = move.getTargets()[1].getPosition(state);
					Vector2i targetPosition = { static_cast<int>(pos.x), static_cast<int>(pos.y) };

					if (pathTilesLookup.find(targetPosition) != pathTilesLookup.end())
					{
						bestAction = move;
						break;
					}
				}
			}

			if (bestAction.getActionFlag() != ActionFlag::EndTickAction)
				break;
		}


		return ActionAssignment::fromSingleAction(bestAction);

	}

	std::vector<std::pair<bool, std::vector<Vector2i>>> PusherAgent::ShortestPaths(const GameState& state, const Vector2i& from, const std::vector<Vector2i>& targets) const
	{
		// Create a index lookup to ensure that we can map an found path to the correct index
		std::unordered_map<Vector2i, int, VectorHash2i> indexLookup;
		for (size_t i = 0; i < targets.size(); i++)
		{
			indexLookup.emplace(targets[i], i);
		}

		std::unordered_map<Vector2i, Vector2i, VectorHash2i> previous; // Lookup for tracing a path backwards
		std::queue<Vector2i> openList;
		openList.push(from);
		std::vector<std::pair<bool, std::vector<Vector2i>>> paths(targets.size());
		int foundCounter = 0;

		// Check if our starting point is already a target
		if (indexLookup.find(from) != indexLookup.end())
		{
			paths[indexLookup[from]].first = true;
			paths[indexLookup[from]].second = {};
		}

		// BFS
		while (openList.size() > 0 && foundCounter < targets.size())
		{
			for (Direction moveDir : CARDINAL_DIRECTIONS)
			{
				// Move from the current tile to the next tile, given by the direction
				Vector2i curr = openList.front();
				auto targetPos = MoveTo(curr, moveDir);
				// Don't explore the current tile if we already visited it or it's invalid
				if (previous.find(targetPos) != previous.end())
					continue;

				// Have we found a path to an target, and is it the first one we find?
				if (indexLookup.find(targetPos) != indexLookup.end() && !paths[indexLookup[targetPos]].first)
				{
					int index = (*indexLookup.find(targetPos)).second;
					// Mark the path as found
					paths[index].first = true;
					// Collect the path to get from our starting tile to the target
					paths[index].second.emplace_back(targetPos);
					while (curr != from)
					{
						paths[index].second.emplace_back(curr);
						curr = previous[curr];
					}

					// Since we traced the path backwards we need to reverse the path to make sure it's in the correct order
					std::reverse(paths[index].second.begin(), paths[index].second.end());

					// We found an path
					foundCounter++;
				}

				// Only explore the new found tile if it's safe to move to, is not occupied by an opponent, and is walkable
				if (state.isInBounds(targetPos) && 
					!canKill(state, targetPos) &&
					(state.getEntityAt({ static_cast<float>(targetPos.x),  static_cast<float>(targetPos.y)}) == nullptr ||
					state.getEntityAt({ static_cast<float>(targetPos.x),  static_cast<float>(targetPos.y) })->getOwnerID() == state.getCurrentTBSPlayer()))
				{
					openList.push(targetPos);
					previous[targetPos] = openList.front();
				}
			}

			// We are done with this position
			openList.pop();
		}

		return paths;
	}

	std::vector<Direction> PusherAgent::GetDangerousDirections(const GameState& state, const Vector2i& pos) const
	{
		std::vector<Direction> dangers;
		for (Direction dir : CARDINAL_DIRECTIONS)
		{
			auto pushPos = MoveTo(pos, dir);
			if (!canKill(state, pushPos))
			{
				auto resultPos = MoveTo(pos, ReverseDir(dir));
				if (canKill(state, resultPos))
				{
					dangers.push_back(dir);
				}
			}
		}

		return dangers;
	}

	bool PusherAgent::canKill(const GameState& state, Vector2i pos) const
	{
		auto tile = state.getTileAt({ pos.x,pos.y });

		if (!tile.isWalkable())
			return false;

		if (tile.name() == "Hole")
			return true;
		else
			return false;
		/*auto targetTile = state.getBoard().getTile(pos.x, pos.y);
		for (auto& effect : onTileEnterEffects)
		{
			if (effect.type == EffectType::Death && effect.targetTileTypeID == targetTile.tileTypeID)
				return true;
		}

		return false;*/
	}

	std::unordered_map<Direction, int> PusherAgent::getAttackDirections(const GameState& state, const Vector2i& pos) const
	{
		std::unordered_map<Direction, int> pushCount;
		for (Direction dir : CARDINAL_DIRECTIONS)
		{
			auto attackPos = MoveTo(pos, dir);
			auto pushDir = ReverseDir(dir);
			auto currPos = MoveTo(pos, pushDir);

			// Check if it is a valid attack direction
			auto& attackTile = state.getTileAt(attackPos.x, attackPos.y);
			auto& pushTile = state.getTileAt(currPos.x, currPos.y);
			if (!attackTile.isWalkable() || canKill(state, attackPos) || !pushTile.isWalkable())
				continue;

			// Count how many pushes are necessary to kill the unit
			int counter = 1;
			for (; state.isInBounds(currPos) && !canKill(state, currPos); counter++)
			{
				currPos = MoveTo(currPos, pushDir);
			}

			pushCount[dir] = counter;
		}

		return pushCount;
	}

	bool PusherAgent::analyzePath(GameState& state, Entity& target, const std::vector<Entity>& opponentUnits, const std::vector<Vector2i>& path, int pushCount, double& pathCostOutput) const
	{
		pathCostOutput = pushCount;
		for (auto i = 0; i < path.size(); i++)
		{
			auto dangers = GetDangerousDirections(state, path[i]);
			bool avoid = false;
			// Check if the next move would put us into danger
			for (auto dangerDir : dangers)
			{
				// Compute the position where we will move to
				auto attackPosition = MoveTo(path[i], dangerDir);
				// Is there an opponent that could kill us?
				for (const auto& opp : opponentUnits)
				{
					auto dist = opp.getPosition().manhattanDistance({ static_cast<float>(attackPosition.x), static_cast<float>(attackPosition.y)});
					// The opponent can kill us if an opponent is standing next to/on an dangerous tile
					// But if it's the unit we want to attack then it's fine to still move there
					if (dist <= 1 && opp.getID() != target.getID())
						avoid = true;
				}
			}

			// Nope this path will kill us in the next step
			if (avoid && i == 0)
				return false;

			// The longer the path the more it costs
			// Also more dangerous tiles increase the cost aswell, anti-proportional to the time it takes to reach them
			pathCostOutput += 1 + avoid * (1 / (i + 1));
		}

		return true;
	}
}