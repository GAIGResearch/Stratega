#include <Agent/RuleBased/PushAgent.h>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <Agent/RuleBased/VectorHash2i.h>

namespace SGA
{
	void PushAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		// Store forwardModel for easier access
		this->forwardModel = forwardModel;
		
		auto currentState = gameCommunicator.getGameState();
		std::vector<TBSAction> actionBucket;
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				auto state = gameCommunicator.getGameState();
				
				// Collect units that we posess and opponent units
				std::vector<TBSUnit> myUnits;
				std::vector<TBSUnit> opponentUnits;
				for (auto& unit : state.getUnits())
				{
					if (unit.getPlayerID() == gameCommunicator.getPlayerID())
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
				auto bestAction = forwardModel.actionSpace->generateEndOfTurnAction(state, gameCommunicator.getPlayerID());
				for (auto& u : myUnits)
				{
					std::vector<Vector2i> possibleTargets;
					std::vector<TBSUnit> attackTarget;
					std::vector<int> pushCount;
					// Compute all positions where we could attack an unit and store it in possibleTargets
					for (auto& opponent : opponentUnits)
					{
						auto attacks = getAttackDirections(state, opponent.getPosition());
						for (auto& attack : attacks)
						{
							auto target = MoveTo(opponent.getPosition(), attack.first);
							if (state.isWalkable(target))
							{
								possibleTargets.emplace_back(target);
								pushCount.emplace_back(attack.second);
								attackTarget.emplace_back(opponent);
							}
						}
					}

					auto paths = ShortestPaths(state, u.getPosition(), possibleTargets);
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
						forwardModel.actionSpace->generatePushActions(u, actionBucket);

						int minimumPushCount = std::numeric_limits<int>::max();
						for (auto& push : actionBucket)
						{
							auto* pushTarget = state.getUnit(push.targetPosition);
							if (pushTarget->getPlayerID() == gameCommunicator.getPlayerID())
								continue; // Do not push our own units

							// We can attack our attack target immediately, so do that
							if (bestAttackIndex != -1 && pushTarget->getUnitID() == attackTarget[bestAttackIndex].getUnitID() && paths[bestAttackIndex].second.empty())
							{
								bestAction = push;
								break;
							}

							// Compute the attack direction from the perspective of the opponent
							auto pushDir = GetDirectionTo(push.targetPosition, u.getPosition());
							// How good is this attack?
							auto attacks = getAttackDirections(state, push.targetPosition);
							if (attacks[pushDir] < minimumPushCount)
							{
								bestAction = push;
								minimumPushCount = attacks[pushDir];
							}
						}
					}

					if (bestAction.type != TBSActionType::EndTurn)
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
						forwardModel.actionSpace->generateMoveActions(u, actionBucket);
						
						for (auto& move : actionBucket)
						{
							if (pathTilesLookup.find(move.targetPosition) != pathTilesLookup.end())
							{
								bestAction = move;
								break;
							}
						}
					}

					if (bestAction.type != TBSActionType::EndTurn)
						break;
				}

				gameCommunicator.executeAction(bestAction);
			}
		}
	}

	std::vector<std::pair<bool,std::vector<Vector2i>>> PushAgent::ShortestPaths(const TBSGameState& state, const Vector2i& from, const std::vector<Vector2i>& targets) const
	{
		// Create a index lookup to ensure that we can map an found path to the correct index
		std::unordered_map<Vector2i, int, VectorHash2i> indexLookup;
		for (int i = 0; i < targets.size(); i++)
		{
			indexLookup.emplace(targets[i], i);
		}

		std::unordered_map<Vector2i, Vector2i, VectorHash2i> previous; // Lookup for tracing a path backwards
		std::queue<Vector2i> openList;
		openList.push(from);
		std::vector<std::pair<bool,std::vector<Vector2i>>> paths(targets.size());
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
				if (!forwardModel.canKill(state, targetPos) && (state.getUnit(targetPos) == nullptr || state.getUnit(targetPos)->getPlayerID() == state.currentPlayer))
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

	/// <summary>
	/// Assuming a unit stands at "pos", this function returns all directions from which another player could kill this unit.
	/// </summary>
	std::vector<Direction> PushAgent::GetDangerousDirections(const TBSGameState& state, const Vector2i & pos) const
	{
		std::vector<Direction> dangers;
		for (Direction dir : CARDINAL_DIRECTIONS)
		{
			auto pushPos = MoveTo(pos, dir);
			if (!forwardModel.canKill(state, pushPos))
			{
				auto resultPos = MoveTo(pos, ReverseDir(dir));
				if (forwardModel.canKill(state, resultPos))
				{
					dangers.push_back(dir);
				}
			}
		}

		return dangers;
	}

	std::unordered_map<Direction, int> PushAgent::getAttackDirections(const TBSGameState& state, const Vector2i & pos) const
	{
		std::unordered_map<Direction, int> pushCount;
		for (Direction dir : CARDINAL_DIRECTIONS)
		{
			auto attackPos = MoveTo(pos, dir);
			auto pushDir = ReverseDir(dir);
			auto currPos = MoveTo(pos, pushDir);

			// Check if it is a valid attack direction
			auto& attackTile = state.getBoard().getTile(attackPos.x, attackPos.y);
			auto& pushTile = state.getBoard().getTile(currPos.x, currPos.y);
			if (!attackTile.isWalkable || forwardModel.canKill(state, attackPos) || !pushTile.isWalkable)
				continue;

			// Count how many pushes are necessary to kill the unit
			int counter = 1;
			for (; state.isInBounds(currPos) && !forwardModel.canKill(state, currPos); counter++)
			{
				currPos = MoveTo(currPos, pushDir);
			}
			
			pushCount[dir] = counter;
		}

		return pushCount;
	}

	bool PushAgent::analyzePath(TBSGameState& state, TBSUnit& target, const std::vector<TBSUnit>& opponentUnits, const std::vector<Vector2i>& path, int pushCount, double& pathCostOutput) const
	{
		pathCostOutput = pushCount;
		for (int i = 0; i < path.size(); i++)
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
					auto dist = opp.getPosition().manhattanDistance(attackPosition);
					// The opponent can kill us if an opponent is standing next to/on an dangerous tile
					// But if it's the unit we want to attack then it's fine to still move there
					if (dist <= 1 && opp.getUnitID() != target.getUnitID())
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
