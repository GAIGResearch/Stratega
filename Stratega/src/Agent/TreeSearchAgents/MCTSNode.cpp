#include"Agent/TreeSearchAgents/MCTSNode.h"

namespace SGA
{
	MCTSNode::MCTSNode(TBSForwardModel& forwardModel, TBSGameState gameState) :
		ITreeNode<SGA::MCTSNode>(forwardModel, std::move(gameState))
	{
	}

	MCTSNode::MCTSNode(TBSForwardModel& forwardModel, TBSGameState gameState, MCTSNode* parent, const int childIndex) :
		ITreeNode<SGA::MCTSNode>(forwardModel, std::move(gameState), parent, childIndex)
	{
	}


	/// <summary>
	/// setting up basic node statistics
	/// </summary>to
	void MCTSNode::initializeNode()
	{
		// initialize node statistics
		if (parentNode) {
			nodeDepth = parentNode->nodeDepth + 1;
			increaseTreeSize();
		}
		else {
			nodeDepth = 0;
		}
	}

	void MCTSNode::setDepth(int depth)
	{
		nodeDepth = depth;
		for (size_t i = 0; i < this->children.size(); i++) {
			children.at(i)->setDepth(depth + 1);
		}
	}

	void MCTSNode::increaseTreeSize()
	{
		treesize++;

		if (parentNode)
		{
			parentNode->increaseTreeSize();
		}
		else
		{
			if (treesize % 500 == 0)
				std::cout << "tree size: " << treesize << "\n";
		}
	}


	/// <summary>
	/// Start MCTS with the provided parameters
	/// </summary>
	/// <param name="params">parameters of the search</param>
	/// <param name="randomGenerator"></param>
	void MCTSNode::searchMCTS(TBSForwardModel& forwardModel, MCTSParameters& params, std::mt19937& randomGenerator) {
		int numIterations = 0;
		bool stop = false;
		int prevCallCount = params.REMAINING_FM_CALLS;

		// stop in case the set number of fmCalls has been reached
		while (!stop) {
			MCTSNode* selected = treePolicy(forwardModel, params, randomGenerator);

			const double delta = selected->rollOut(forwardModel, params, randomGenerator);
			//cout << "delta: " << delta << "\n";
			backUp(selected, delta);
			numIterations++;
			//printTree();

			stop = params.REMAINING_FM_CALLS <= 0 || numIterations == params.MAX_FM_CALLS;
			prevCallCount = params.REMAINING_FM_CALLS;
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
	MCTSNode* MCTSNode::treePolicy(TBSForwardModel& forwardModel, MCTSParameters& params, std::mt19937& randomGenerator)
	{
		MCTSNode* cur = this;

		while (!cur->gameState.isGameOver)// && cur->nodeDepth < params.ROLLOUT_LENGTH)
		{
			if (!cur->isFullyExpanded()) {
				return (cur->expand(forwardModel, params, randomGenerator));
			}
			else {
				//printTree();
				cur = cur->uct(params, randomGenerator);
			}
		}
		return cur;
	}

	MCTSNode* MCTSNode::expand(TBSForwardModel& forwardModel, MCTSParameters& params, std::mt19937& randomGenerator)
	{
		// roll the state
		//todo remove unnecessary copy of gameState
		TBSGameState gsCopy = TBSGameState(gameState);
		childIndex = children.size();
		applyActionToGameState(forwardModel, gsCopy, actionSpace.at(childIndex), params);

		// generate child node and add it to the tree
		children.push_back(std::unique_ptr<MCTSNode>(new MCTSNode(forwardModel, std::move(gsCopy), this, childIndex)));

		return children[childIndex].get();
	}

	double MCTSNode::normalize(const double aValue, const double aMin, const double aMax)
	{
		if (aMin < aMax)
			return (aValue - aMin) / (aMax - aMin);

		// if bounds are invalid, then return same value
		return aValue;
	}

	double MCTSNode::noise(const double input, const double epsilon, const double random)
	{
		return (input + epsilon) * (1.0 + epsilon * (random - 0.5));
	}

	MCTSNode* MCTSNode::uct(MCTSParameters& params, std::mt19937& randomGenerator)
	{
		const bool iAmMoving = (gameState.currentPlayer == params.PLAYER_ID);

		std::vector<double> childValues(children.size(), 0);

		for (size_t i = 0; i < children.size(); ++i)
		{
			MCTSNode* child = children[i].get();

			const double hvVal = child->value;
			double childValue = hvVal / (child->nVisits + params.EPSILON);
			childValue = normalize(childValue, bounds[0], bounds[1]);

			double uctValue = childValue +
				params.K * sqrt(log(this->nVisits + 1) / (child->nVisits + params.EPSILON));

			uctValue = noise(uctValue, params.EPSILON, params.doubleDistribution_(randomGenerator));     //break ties randomly
			childValues[i] = uctValue;
		}

		int which = -1;
		double bestValue = iAmMoving ? -std::numeric_limits<double>::max() : std::numeric_limits<double>::max();

		for (int i = 0; i < children.size(); ++i) {
			if ((iAmMoving && childValues[i] > bestValue) || (!iAmMoving && childValues[i] < bestValue)) {
				which = i;
				bestValue = childValues[i];
			}
		}

		if (which == -1)
		{
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
			std::cout << nodeDepth << ", AmIMoving? " << iAmMoving << "\n";

			for (size_t i = 0; i < children.size(); ++i)
				std::cout << "\t" << childValues[i] << "\n";
			std::cout << "; selected: " << which << "\n";
			std::cout << "; isFullyExpanded: " << isFullyExpanded() << "\n";
			std::uniform_int_distribution<> distrib(0, children.size() - 1);

			which = distrib(randomGenerator);
		}

		return children[which].get();
	}

	double MCTSNode::rollOut(TBSForwardModel& forwardModel, MCTSParameters& params, std::mt19937& randomGenerator)
	{
		if (params.ROLLOUTS_ENABLED) {
			TBSGameState gsCopy = TBSGameState(gameState);
			int thisDepth = nodeDepth;

			while (!(rolloutFinished(gsCopy, thisDepth, params) || gsCopy.isGameOver)) {
				auto actions = forwardModel.getActions(gsCopy);
				if (actions.size() == 0)
					break;
				std::uniform_int_distribution<> randomDistribution(0, actions.size() - 1);
				applyActionToGameState(forwardModel, gsCopy, actions.at(randomDistribution(randomGenerator)), params);
				thisDepth++;
			}
			return normalize(params.STATE_HEURISTIC->evaluateGameState(forwardModel, gsCopy, params.PLAYER_ID), 0, 1);
		}

		return normalize(params.STATE_HEURISTIC->evaluateGameState(forwardModel, gameState, params.PLAYER_ID), 0, 1);
	}

	bool MCTSNode::rolloutFinished(TBSGameState& rollerState, int depth, MCTSParameters& params)
	{
		if (depth >= params.ROLLOUT_LENGTH)      //rollout end condition.
			return true;

		//end of game
		return rollerState.isGameOver;
	}

	void MCTSNode::applyActionToGameState(TBSForwardModel& forwardModel, TBSGameState& gameState, TBSAction& action, MCTSParameters& params) const
	{
		params.REMAINING_FM_CALLS--;
		forwardModel.advanceGameState(gameState, action);
		while (gameState.currentPlayer != params.PLAYER_ID && !gameState.isGameOver)
		{
			if (params.opponentModel) // use default opponentModel to choose actions until the turn has ended
			{
				params.REMAINING_FM_CALLS--;
				auto actionSpace = forwardModel.getActions(gameState);
				auto opAction = params.opponentModel->getAction(gameState, actionSpace);
				forwardModel.advanceGameState(gameState, opAction);
			}
			else // skip opponent turn
			{
				std::vector<TBSAction> endTurnActionSpace;
				forwardModel.getActionSpace().generateEndOfTurnActions(gameState, gameState.currentPlayer, endTurnActionSpace);
				forwardModel.advanceGameState(gameState, endTurnActionSpace.at(0));
			}
		}
	}

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

	int MCTSNode::mostVisitedAction(MCTSParameters& params, std::mt19937& randomGenerator)
	{
		int selected = -1;
		double bestValue = -std::numeric_limits<double>::max();
		bool allEqual = true;
		double first = -1;

		//cout << "Remaining budget: " << params.REMAINING_FM_CALLS << "\n";
		//printTree();

		//auto* childValues = new double[children.size()];
		std::vector<double> childValues(children.size(), 0);
		for (size_t i = 0; i < children.size(); ++i)
		{
			MCTSNode* child = children[i].get();

			const double hvVal = child->value;
			childValues[i] = hvVal;
		}

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
				//double childValue = children[i]->totValue / children[i]->nVisits ;
				childValue = noise(childValue, params.EPSILON, params.doubleDistribution_(randomGenerator));     //break ties randomly
				if (childValue > bestValue) {
					bestValue = childValue;
					selected = i;
				}
			}
		}

		if (selected == -1)
		{
			selected = 0;
		}
		else if (allEqual)
		{
			//If all are equal, we opt to choose for the one with the best Q.
			selected = bestAction(params, randomGenerator);
		}
		//cout << "best action: " << actions.at(selected)->getName() << "\n";

		return selected;
	}

	int MCTSNode::bestAction(MCTSParameters& params, std::mt19937& randomGenerator)
	{
		int selected = -1;
		double bestValue = -std::numeric_limits<double>::max();

		for (size_t i = 0; i < children.size(); i++) {

			if (children[i] != nullptr) {
				double childValue = children[i]->value / (children[i]->nVisits + params.EPSILON);
				childValue = noise(childValue, params.EPSILON, params.doubleDistribution_(randomGenerator));     //break ties randomly
				if (childValue > bestValue) {
					bestValue = childValue;
					selected = i;
				}
			}
		}

		if (selected == -1)
		{
			//cout << "Unexpected selection!" << "\n";
			selected = 0;
		}

		return selected;
	}

	void MCTSNode::print() const
	{
		std::cout << this->value / this->nVisits << "; "<< this->nVisits << "; " << children.size();
	}
	
}
