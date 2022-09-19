#include <Stratega/Agent/MCTSAgent/MCTSNode.h>
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	MCTSNode::MCTSNode(ForwardModel& forwardModel, GameState newGameState, int newOwnerID) :
		ITreeNode<SGA::MCTSNode>(forwardModel, std::move(newGameState), newOwnerID)
	{
        childExpanded = 0;
	}

	MCTSNode::MCTSNode(ForwardModel& forwardModel, GameState newGameState, MCTSNode* newParent, const int newChildIndex, int newOwnerID) :
		ITreeNode<SGA::MCTSNode>(forwardModel, std::move(newGameState), newParent, newChildIndex, newOwnerID)
	{
		computeActionSpace(forwardModel);
		initializeNode();
	}


	/// <summary>
	/// setting up basic node statistics
	/// </summary>to
	void MCTSNode::initializeNode()
	{
		if (parentNode) {
			nodeDepth = parentNode->nodeDepth + 1;
			increaseTreeSize();
		}
		else {
			nodeDepth = 0;
		}
        childExpanded = 0;
	}

	/// <summary>
	/// Start MCTS with the provided parameters
	/// </summary>
	/// <param name="params">parameters of the search</param>
	/// <param name="randomGenerator"></param>
	void MCTSNode::searchMCTS(ForwardModel& forwardModel, MCTSParameters& params, boost::mt19937& randomGenerator) {
		// stop in case the budget is over.
		MCTSNode* last_selected = nullptr;
		int n_repeat_selection = 0;
        int tmp_fm_call_spent = 0;
        int fm_call_not_advance = 0;
		while (!params.isBudgetOver()) {

			MCTSNode* selected = treePolicy(forwardModel, params, randomGenerator);
			if (last_selected != nullptr) {
				if (last_selected == selected)
					n_repeat_selection++;
				else
					n_repeat_selection = 0;
			}
			last_selected = selected;
            tmp_fm_call_spent = params.currentFMCalls;
			double delta = selected->rollOut(forwardModel, params, randomGenerator);

            // rollout does not spent forward model
            if(params.budgetType == Budget::FMCALLS){
                if(params.currentFMCalls == tmp_fm_call_spent){
                    fm_call_not_advance++;
                    if(fm_call_not_advance > 20)return;
                }
                else {
                    fm_call_not_advance = 0;
                }
            }
			backUp(selected, delta);
			params.currentIterations++;
			if (n_repeat_selection >= 20) // repeated selection
				return;
            if (params.currentIterations > 1000)
                return;
            //std::cout<< "n_repeat_selection: "<< n_repeat_selection<< " fm calls: "<< params.currentFMCalls << "\n";
		}
	}

	/// <summary>
	/// Select the node to be expanded next.
	/// Apply UCT until a node has been found that is not fully expanded yet
	/// or the maximum depth has been reached.
	/// </summary>
	/// <param name="params">parameters of the search</param>
	/// <param name="randomGenerator"></param>
	/// <returns></returns>
	MCTSNode* MCTSNode::treePolicy(ForwardModel& forwardModel, MCTSParameters& params, boost::mt19937& randomGenerator)
	{
		MCTSNode* cur = this;

		while (!cur->gameState.isGameOver() && cur->nodeDepth < params.rolloutLength)
		{
			//If not fully expanded, add a new child
			if (!cur->isFullyExpanded()) {
				return (cur->expand(forwardModel, params, randomGenerator));
			}
			else {
				//otherwise apply UCT to navigate the tree.
				cur = cur->uct(params, randomGenerator);
			}
		}
		return cur;
	}

	MCTSNode* MCTSNode::expand(ForwardModel& forwardModel, MCTSParameters& params, boost::mt19937& /*randomGenerator*/)
	{
		auto gsCopy(gameState);
		auto action = actionSpace.at(children.size());
		applyActionToGameState(forwardModel, gsCopy, action, params, playerID);

		// generate child node and add it to the tree
		children.push_back(std::unique_ptr<MCTSNode>(new MCTSNode(forwardModel, std::move(gsCopy), this, childExpanded, this->ownerID)));
        //std::cout<< children.size() << " childExapnded: "<< childExpanded<<std::endl;
		childExpanded ++;

		return children[static_cast<size_t>(children.size()-1)].get();
	}

	MCTSNode* MCTSNode::uct(MCTSParameters& params, boost::mt19937& randomGenerator)
	{
		//Find out if this node corresponds to a state where I can move.
		bool amIMoving = gameState.canPlay(params.PLAYER_ID);
		std::vector<double> childValues(children.size(), 0);

		for (size_t i = 0; i < children.size(); ++i)
		{
			MCTSNode* child = children[i].get();

			//Compute the value of the child. First the exploitation value:
			const double hvVal = child->value;
			double childValue = hvVal / (child->nVisits + params.epsilon);
			//childValue = normalize(childValue, bounds[0], bounds[1]);

			//Then add the exploration factor multiplied by constant K.
			double uctValue = childValue +
				params.K * sqrt(log(this->nVisits + 1) / (child->nVisits + params.epsilon));

			//Add a small noise to break ties randomly
			uctValue = noise(uctValue, params.epsilon, params.doubleDistribution_(randomGenerator));
			childValues[i] = uctValue;
		}

		//Keep the best action as decided by UCT. If I'm moving in this node, we'll aim to maximize the UCT value. Otherwise, minimize. 
		int which = -1;
		double bestValue = amIMoving ? -std::numeric_limits<double>::max() : std::numeric_limits<double>::max();

		for (size_t i = 0; i < children.size(); ++i) {
			if ((amIMoving && childValues[i] > bestValue) || (!amIMoving && childValues[i] < bestValue)) {
				which = static_cast<int>(i);
				bestValue = childValues[i];
			}
		}

		if (which == -1)
		{
			//This shouldn't happen, MCTS can't find a node to pick. Use this printing for debug:
			std::cout << "this subtree:" << "\n";
			printTree();

			std::cout << "\n\n" << "the whole tree:" << "\n";
			MCTSNode* root = this;
			while (root->parentNode != nullptr)
			{
				root = root->parentNode;
			}
			printTree("", root, true, "root");
			std::cout << "\n\n";

			//if(this.children.length == 0)
			std::cout << "Warning! couldn't find the best UCT value " << which << " : " << children.size() << "\n";
			std::cout << nodeDepth << ", amIMoving? " << amIMoving << "\n";

			for (size_t i = 0; i < children.size(); ++i)
				std::cout << "\t" << childValues[i] << "\n";
			std::cout << "; selected: " << which << "\n";
			std::cout << "; isFullyExpanded: " << isFullyExpanded() << "\n";
			boost::random::uniform_int_distribution<size_t> distrib(0, children.size() - 1);

			which = static_cast<int>(distrib(randomGenerator));
		}

		return children[static_cast<size_t>(which)].get();
	}

	//Executes the rollout phase of MCTS
	double MCTSNode::rollOut(ForwardModel& forwardModel, MCTSParameters& params, boost::mt19937& randomGenerator)
	{
		//... only if rollouts are enabled in the parameter settings.
		if (params.rolloutsEnabled) {

			//Create a copy and mark our depth on the tree.
			auto gsCopy(gameState);
			int thisDepth = nodeDepth;

            // [BUGfixed] for FMCALLS budget: selected state is a terminal state that cost no budget
            if (gsCopy.isGameOver() && params.budgetType == Budget::FMCALLS) {
                //std::cout<<"catch terminal state!\n";
                params.currentFMCalls ++;
            }

			//If we must keep rolling.
			while (!(rolloutFinished(gsCopy, thisDepth, params) || gsCopy.isGameOver())) {

				//Find my action space.
				auto actions = forwardModel.generateActions(gsCopy, params.PLAYER_ID);
				if (actions.size() == 0)
					break;

				//Pick one action at random and apply it to the current game state. 
				boost::random::uniform_int_distribution<size_t> randomDistribution(0, actions.size() - 1);
				applyActionToGameState(forwardModel, gsCopy, actions.at(randomDistribution(randomGenerator)), params, playerID);
				thisDepth++;
			}

			//We evaluate the state at the end of the rollout using the heuristic specified in the parameter settings. 
			//We then return this reward to the last node expanded in the tree.
			//return normalize(params.heuristic->evaluateGameState(forwardModel, gsCopy, params.PLAYER_ID), 0, 1);
			return params.heuristic->evaluateGameState(forwardModel, gsCopy, params.PLAYER_ID);
		}

		return params.heuristic->evaluateGameState(forwardModel, gameState, params.PLAYER_ID);
	}

	bool MCTSNode::rolloutFinished(GameState& rollerState, int depth, MCTSParameters& params)
	{
        if (depth >= params.rolloutLength) {
            //std::cout<<"depth out of range\n";
            return true;
        }
			

		return rollerState.isGameOver();
	}

	void MCTSNode::applyActionToGameState(ForwardModel& forwardModel, GameState& targetGameState, Action& action, MCTSParameters& params, int /*playerID*/) const
	{
		//Roll the game state with our action.
		//params.currentFMCalls += SGA::roll(targetGameState, forwardModel, action, playerID, params);
        int rollReturn = SGA::roll(targetGameState, forwardModel, action, playerID, params);
        //std::cout<<rollReturn << std::endl;
        params.currentFMCalls += rollReturn;

		//Continue rolling the state until the game is over, we run out of budget or this agent can play again. 
		while (!targetGameState.canPlay(params.PLAYER_ID) && !params.isBudgetOver() && !targetGameState.isGameOver())
		{
			//Roll actions for the opponent(s).
			params.currentFMCalls += SGA::rollOppOnly(targetGameState, forwardModel, params);
		}
	}

	//Backpropagation in MCTS. Update number of visits, accummulated reward value and node reward bounds.
	void MCTSNode::backUp(MCTSNode* node, const double result)
	{
		MCTSNode* n = node;
		while (n != nullptr)
		{
			n->nVisits++;
			n->value += result;
			if (result < n->bounds[0]) {
				n->bounds[0] = result;
			}
			if (result > n->bounds[1]) {
				n->bounds[1] = result;
			}
			n = n->parentNode;
		}
	}

	//Returns the index of the most visited child of this node.
	int MCTSNode::mostVisitedAction(MCTSParameters& params, boost::mt19937& randomGenerator)
	{
		int selected = -1;
		double bestValue = -std::numeric_limits<double>::max();
		bool allEqual = true;
		double first = -1;

		//See the visits to all children, keep the one with the highest.
		for (size_t i = 0; i < children.size(); i++) {

			if (children[i] != nullptr)
			{
				if (first == -1)
					first = children[i]->nVisits;
				else if (first != children[i]->nVisits)
				{
					allEqual = false;
				}

				double childValue = children[i]->nVisits;

				//Add a small noise (<<1) to all values to break ties randomly
				childValue = noise(childValue, params.epsilon, params.doubleDistribution_(randomGenerator));
				if (childValue > bestValue) {
					bestValue = childValue;
					selected = static_cast<int>(i);
				}
			}
		}

		if (selected == -1 || allEqual)
		{
			//If all are equal, we opt to choose for the one with the best Q.
			selected = bestAction(params, randomGenerator);
		}

		return selected;
	}

	//Selects the index of the child with the highest average reward value.
	int MCTSNode::bestAction(MCTSParameters& params, boost::mt19937& randomGenerator)
	{
		int selected = -1;
		double bestValue = -std::numeric_limits<double>::max();

		//Check the values for all children.
		for (size_t i = 0; i < children.size(); i++) {

			if (children[i] != nullptr) {
				double childValue = children[i]->value / (children[i]->nVisits + params.epsilon);

				//Add a small random noise to break ties randomly
				childValue = noise(childValue, params.epsilon, params.doubleDistribution_(randomGenerator));
				if (childValue > bestValue) {
					bestValue = childValue;
					selected = static_cast<int>(i);
				}
			}
		}

		if (selected == -1)
		{
			//This shouldn't happen, just pick the first action.
			//cout << "Unexpected selection!" << "\n";
			selected = 0;
		}

		return selected;
	}

	// helper function: normalizes a value between a range aMin - aMax.
	double MCTSNode::normalize(const double aValue, const double aMin, const double aMax)
	{
		if (aMin < aMax)
			return (aValue - aMin) / (aMax - aMin);

		// if bounds are invalid, then return same value
		return aValue;
	}

	// helper function: adds a small random noise to a value and returns it.
	double MCTSNode::noise(const double input, const double epsilon, const double random)
	{
		return (input + epsilon) * (1.0 + epsilon * (random - 0.5));
	}

	// setter for the depth of this node and all nodes in the sub-tree this is root of.
	void MCTSNode::setDepth(int depth)
	{
		nodeDepth = depth;
		for (size_t i = 0; i < this->children.size(); i++) {
			children.at(i)->setDepth(depth + 1);
		}
	}

	// Increments the counter of nodes below this one.
	void MCTSNode::increaseTreeSize()
	{
		treesize++;
		if (parentNode)
		{
			parentNode->increaseTreeSize();
		}
		else
		{
			//if (treesize % 500 == 0)
			//	std::cout << "tree size: " << treesize << "\n";
		}
	}

    void MCTSNode::printActionInfo() const {
        if (this->parentNode != nullptr) {
            auto a = this->parentNode->actionSpace[this->childIndex];
            this->parentNode->gameState.printActionInfo(a);
        }
    }

	void MCTSNode::print() const
	{
		std::cout << "value: "<< this->value / this->nVisits << "; nVisit: " << this->nVisits << "; children size: " << children.size() 
            << "; depth: "<< nodeDepth << " ";
        printActionInfo();
	}

}