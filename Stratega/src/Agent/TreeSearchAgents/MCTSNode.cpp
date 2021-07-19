#include <Stratega/Agent/TreeSearchAgents/MCTSNode.h>
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	MCTSNode::MCTSNode(ForwardModel& forwardModel, GameState gameState, int ownerID) :
		ITreeNode<SGA::MCTSNode>(forwardModel, std::move(gameState), ownerID)
	{
	}

	MCTSNode::MCTSNode(ForwardModel& forwardModel, GameState gameState, MCTSNode* parent, const int childIndex, int ownerID) :
		ITreeNode<SGA::MCTSNode>(forwardModel, std::move(gameState), parent, childIndex, ownerID)
	{
		computeActionSpace(forwardModel);
		initializeNode();
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
	void MCTSNode::searchMCTS(ForwardModel& forwardModel, MCTSParameters& params, std::mt19937& randomGenerator) {
		// stop in case the set number of fmCalls has been reached
		while (!params.isBudgetOver()) {
			MCTSNode* selected = treePolicy(forwardModel, params, randomGenerator);
			const double delta = selected->rollOut(forwardModel, params, randomGenerator);
			backUp(selected, delta);
			params.currentIterations++; 
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
	MCTSNode* MCTSNode::treePolicy(ForwardModel& forwardModel, MCTSParameters& params, std::mt19937& randomGenerator)
	{
		MCTSNode* cur = this;

		while (!cur->gameState.isGameOver())// && cur->nodeDepth < params.rolloutLength)
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

	MCTSNode* MCTSNode::expand(ForwardModel& forwardModel, MCTSParameters& params, std::mt19937& /*randomGenerator*/)
	{
		// roll the state
		//todo remove unnecessary copy of gameState
		auto gsCopy(gameState);
		auto action = actionSpace.at(static_cast<int>(children.size()));
		applyActionToGameState(forwardModel, gsCopy, action, params, playerID);

		// generate child node and add it to the tree
		children.push_back(std::unique_ptr<MCTSNode>(new MCTSNode(forwardModel, std::move(gsCopy), this, childIndex, this->ownerID)));
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
		bool amIMoving = gameState.canPlay(params.PLAYER_ID);

		std::vector<double> childValues(children.size(), 0);

		for (size_t i = 0; i < children.size(); ++i)
		{
			MCTSNode* child = children[i].get();

			const double hvVal = child->value;
			double childValue = hvVal / (child->nVisits + params.epsilon);
			childValue = normalize(childValue, bounds[0], bounds[1]);

			double uctValue = childValue +
				params.K * sqrt(log(this->nVisits + 1) / (child->nVisits + params.epsilon));

			uctValue = noise(uctValue, params.epsilon, params.doubleDistribution_(randomGenerator));     //break ties randomly
			childValues[i] = uctValue;
		}

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
			std::uniform_int_distribution<size_t> distrib(0, children.size() - 1);

			which = static_cast<int>(distrib(randomGenerator));
		}

		return children[which].get();
	}

	double MCTSNode::rollOut(ForwardModel& forwardModel, MCTSParameters& params, std::mt19937& randomGenerator)
	{
		if (params.rolloutsEnabled) {
			auto gsCopy(gameState);
			int thisDepth = nodeDepth;

			while (!(rolloutFinished(gsCopy, thisDepth, params) || gsCopy.isGameOver())) {
				auto actions = forwardModel.generateActions(gsCopy, params.PLAYER_ID);
				if (actions.size() == 0)
					break;
				std::uniform_int_distribution<size_t> randomDistribution(0, actions.size() - 1);
				applyActionToGameState(forwardModel, gsCopy, actions.at(randomDistribution(randomGenerator)), params, playerID);
				thisDepth++;
			}
			return normalize(params.heuristic->evaluateGameState(forwardModel, gsCopy, params.PLAYER_ID), 0, 1);
		}

		return normalize(params.heuristic->evaluateGameState(forwardModel, gameState, params.PLAYER_ID), 0, 1);
	}

	bool MCTSNode::rolloutFinished(GameState& rollerState, int depth, MCTSParameters& params)
	{
		if (depth >= params.rolloutLength)      //rollout end condition.
			return true;

		//end of game
		return rollerState.isGameOver();
	}

	void MCTSNode::applyActionToGameState(ForwardModel& forwardModel, GameState& targetGameState, Action& action, MCTSParameters& params, int playerID) const
	{
		//Roll the game state with our action.
		params.currentFMCalls += SGA::roll(targetGameState, forwardModel, action, playerID, params);

		//Continue rolling the state until the game is over, we run out of budget or this agent can play again. 
		while (!targetGameState.canPlay(params.PLAYER_ID) && !params.isBudgetOver() && !targetGameState.isGameOver())
		{
			//Roll actions for the opponent(s).
			params.currentFMCalls += SGA::rollOppOnly(targetGameState, forwardModel, params);
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
				childValue = noise(childValue, params.epsilon, params.doubleDistribution_(randomGenerator));     //break ties randomly
				if (childValue > bestValue) {
					bestValue = childValue;
					selected = static_cast<int>(i);
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
				double childValue = children[i]->value / (children[i]->nVisits + params.epsilon);
				childValue = noise(childValue, params.epsilon, params.doubleDistribution_(randomGenerator));     //break ties randomly
				if (childValue > bestValue) {
					bestValue = childValue;
					selected = static_cast<int>(i);
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
