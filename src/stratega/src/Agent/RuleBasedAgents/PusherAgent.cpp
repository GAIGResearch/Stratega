#include <Stratega/Agent/RuleBasedAgents/PusherAgent.h>

namespace SGA
{

    void PusherAgent::initialize(GameState state) {
        int height = state.getBoardHeight();
        int width = state.getBoardWidth();
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                auto tile = state.getTileAt(Vector2i(i,j));
                if (tile.name() == "Hole") {
                    hole_pos_list.push_back(Vector2f(i,j));
                }
            }
        }
        
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
            //std::cout<< actionTypeIDToActionTypeString.at(a.getActionTypeID())<<"\n";
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
        if (!isInitialized) {
            initialize(state);
            isInitialized = true;
        }
		return playTurn(state, forwardModel);
	}

	ActionAssignment PusherAgent::playTurn(GameState& newState, const ForwardModel& forwardModel)
	{
        //newState.printBoard();
        // probability of taking random actions
        if (dis(getRNGEngine()) > 0.97) {//87
            //std::cout<<"enter random action\n";
            auto actions = forwardModel.generateActions(newState, getPlayerID());
            boost::random::uniform_int_distribution<size_t> actionDist(0, actions.size() - 1);
            auto actionIndex = actionDist(getRNGEngine());
            auto action = actions.at(actionIndex);
            //std::cout<<"select random agent\n";
            return ActionAssignment::fromSingleAction(action);
        }
        //std::cout<<"Start PusherAgent\n";

        // store name of actions
		for (const auto& a : newState.getGameInfo()->getActionTypes())
		{
			actionTypeIDToActionTypeString[a.first] = a.second.getName();
		}
		actionTypeIDToActionTypeString[-1] = "EndTurn";

		auto currentState = newState;
		std::vector<Action> actionBucket;

		auto state = newState;
        //std::cout<<"00\n";

		// Collect units that we control and opponent units
		std::vector<Entity> myUnits;
        std::vector<Entity> myGhost;
		std::vector<Entity> opponentUnits;
        
		for (auto& unit : state.getEntities())
		{
			if (unit.getOwnerID() == this->getPlayerID())
			{
                if (state.getGameInfo()->getEntityType(unit.getEntityTypeID()).getName() == "Ghost") {
                    myGhost.push_back(unit);
                }
                else {
                    myUnits.push_back(unit);
                }
			}
			else if (unit.getOwnerID() == -this->getPlayerID()+1)// player ID = {0,1}
			{
				opponentUnits.push_back(unit);
            }
            else { // hole is not an entity

            }
		}

        // Go through all units and return the first action that we deem good
        // Since this function is called multiple times, we will eventually use up all actions available
        auto actions = forwardModel.generateActions(currentState, getPlayerID());

        // initialize the best action
        auto action_candidate = filterActionTypes(actions, "EndTurn");
        Action bestAction = action_candidate.at(0);

        // execute empty for all out ghost
        /*for (auto& u : myGhost) {
            auto unit_action_space = forwardModel.generateUnitActions(state, u, this->getPlayerID(), false);
            if (unit_action_space.size() == 0) {
                continue;
            }
            else {
                auto empty_a = filterActionTypes(unit_action_space, "Empty");
                //std::cout<<"empty_a.size(): "<< empty_a.size()<<"\n";
                return ActionAssignment::fromSingleAction(empty_a.at(0));
            }
        }*/

        //std::cout<<"uu\n";
        for (auto& u : myUnits)
        {
            auto unitActionSpace = forwardModel.generateUnitActions(state, u, getPlayerID(), false);
            if(unitActionSpace.size() == 0)continue;

            auto my_pos = u.getPosition();
            Entity target = u;
            double min_dis = 1000.0;
            double tmp_dis = 1000.0;
            // find the target unit and approaching 
            for (auto opp_u : opponentUnits) {
                tmp_dis = my_pos.distance(opp_u.getPosition());
                if ( tmp_dis < min_dis) {
                    min_dis = tmp_dis;
                    target = opp_u;
                }
            }

            if (!(min_dis < 1.1)) { //if distance < 1.5 don't move
                auto moveActionSpace = filterActionTypes(unitActionSpace, "Move");
                for (auto moveA : moveActionSpace) {
                    auto gsCopy(state);
                    forwardModel.advanceGameState(gsCopy, moveA);
                    auto new_entity = gsCopy.getEntity(u.getID());
                    if (new_entity == nullptr) {
                        continue;
                    }
                    auto pos_next = new_entity->getPosition();
                    if (pos_next.distance(target.getPosition()) < min_dis) {
                        //std::cout<<"move \n";

                        return ActionAssignment::fromSingleAction(moveA);
                    }
                }
            }
            else {
                auto empty_a = filterActionTypes(unitActionSpace, "Empty");
                if (empty_a.size() > 0) {
                    //std::cout<<"empty \n";
                    return ActionAssignment::fromSingleAction(empty_a.at(0));
                }
                //std::cout<<"empty \n";
                //for (auto a : unitActionSpace) {
                //    state.printActionInfo(a);
                //}
                
            }
            
            // find the target hole
            Vector2f tmp_h_pos;
            tmp_dis = 1000.0;
            double tmp_target_hole_dis = 0;
            for (auto h_pos : hole_pos_list) {
                tmp_target_hole_dis = h_pos.distance(target.getPosition() );
                if ( tmp_target_hole_dis < tmp_dis) {
                    tmp_h_pos = h_pos;
                    tmp_dis = tmp_target_hole_dis;
                }
            }
            
            //std::cout<<"234908 \n";

            //if code goes here, it means this unit can push Q: why there is a situation that not all pushdirection exists
            std::vector<Action> pushAction = {};
            if (filterActionTypes(unitActionSpace, "PushLeft").size() > 0)
                pushAction.push_back(filterActionTypes(unitActionSpace, "PushLeft")[0]);
            if(filterActionTypes(unitActionSpace, "PushRight").size()>0)
                pushAction.push_back(filterActionTypes(unitActionSpace, "PushRight")[0]);
            if(filterActionTypes(unitActionSpace, "PushDown").size()>0)
                pushAction.push_back(filterActionTypes(unitActionSpace, "PushDown")[0]);
            if(filterActionTypes(unitActionSpace, "PushUp").size()>0)
                pushAction.push_back(filterActionTypes(unitActionSpace, "PushUp")[0]);
            if (pushAction.size() > 0) {
                //std::cout<<"8f9wehjf \n";
                for (auto pushA : pushAction) {
                    auto gsCopy(state);
                    forwardModel.advanceGameState(gsCopy, pushA);
                    auto newUnit = gsCopy.getEntity(target.getID());
                    if (newUnit == nullptr) {
                        //std::cout<<"push \n";
                        return ActionAssignment::fromSingleAction(pushA);
                    }

                    auto pos_next = newUnit->getPosition();
                    if (pos_next.distance(tmp_h_pos) < tmp_dis) {
                        //std::cout<<"push \n";
                        return ActionAssignment::fromSingleAction(pushA);
                    }
                }
                // here means no push can make enemy closer
                return ActionAssignment::fromSingleAction(pushAction[0]);
            }

            // push to the right direction
        }
        //std::cout<<"fioewif \n";

        //state.printBoard();
        // all unit moves, end the turn
        bestAction = Action::createEndAction(getPlayerID());

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
		while (static_cast<int>(openList.size()) > 0 && foundCounter < static_cast<int>(targets.size()))
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
		const auto tile = state.getTileAt({ pos.x,pos.y });

		if (!tile.isWalkable())
			return false;

		if (tile.name() == "Hole")
			return true;
		else
			return false;
	}

	std::unordered_map<Direction, int> PusherAgent::getAttackDirections(const GameState& state, const Vector2i& pos) const
	{
        // returns an direction value function
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
		for (size_t i = 0; i < path.size(); i++)
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
			pathCostOutput += static_cast<double>(1 + avoid * (1 / (i + 1)));
		}

		return true;
	}
}