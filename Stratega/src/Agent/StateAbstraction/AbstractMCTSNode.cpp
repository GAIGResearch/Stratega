#include <Stratega/Agent/StateAbstraction/AbstractMCTSNode.h>


namespace SGA
{
	AbstractMCTSNode::AbstractMCTSNode(TBSForwardModel& forwardModel, AbstractState abstractState, GameState gameState) :
		IAbstractTreeNode<AbstractMCTSNode>(forwardModel, std::move(abstractState), std::move(gameState))
	{
	}

	AbstractMCTSNode::AbstractMCTSNode(TBSForwardModel& forwardModel, AbstractState abstractState, GameState gameState, AbstractMCTSNode* parent, const int childIndex) :
		IAbstractTreeNode<AbstractMCTSNode>(forwardModel, std::move(abstractState), std::move(gameState), parent, childIndex)
	{
	}


	/// <summary>
	/// setting up basic node statistics
	/// </summary>to
	void AbstractMCTSNode::initializeNode()
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

	void AbstractMCTSNode::setDepth(int depth)
	{
		nodeDepth = depth;
		for (size_t i = 0; i < this->children.size(); i++) {
			children.at(i)->setDepth(depth + 1);
		}
	}

	void AbstractMCTSNode::increaseTreeSize()
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
	void AbstractMCTSNode::searchMCTS(TBSForwardModel& forwardModel, AbstractMCTSParameters& params, std::mt19937& randomGenerator) {
		int numIterations = 0;
		bool stop = false;
		int prevCallCount = params.REMAINING_FM_CALLS;

		// stop in case the set number of fmCalls has been reached
		while (!stop) {
			AbstractMCTSNode* selected = treePolicy(forwardModel, params, randomGenerator);

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
	AbstractMCTSNode* AbstractMCTSNode::treePolicy(TBSForwardModel& forwardModel, AbstractMCTSParameters& params, std::mt19937& randomGenerator)
	{
		AbstractMCTSNode* cur = this;

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

	AbstractMCTSNode* AbstractMCTSNode::expand(TBSForwardModel& forwardModel, AbstractMCTSParameters& params, std::mt19937& randomGenerator)
	{
		// roll the state
		//todo remove unnecessary copy of gameState
		auto gsCopy(gameState);
		int actionIndex = actionToChildIndex.size();
		
		applyActionToGameState(forwardModel, gsCopy, actionSpace.at(actionIndex), params);

		auto abstractState = params.STATE_FACTORY->createAbstractState(gsCopy);

		// check if state already exists as a child
		auto current = children.begin();
		const auto end = children.end();
		while (current != end) {
			if ((*current)->abstractGameState == abstractState) break;
			++current;
		}
		if (current != end)
		{
			// the gameState already exists
			actionToChildIndex.push_back((*current)->childIndex);
			
			// add the action to the current nodes child mapping and return the existing state
			return (*current).get();
		}
		else
		{
			// it is a new abstract gameState
			// generate child node, add it to the tree, and link action to childNode
			const int newChildIndex = children.size();
			children.push_back(std::unique_ptr<AbstractMCTSNode>(new AbstractMCTSNode(forwardModel, std::move(abstractState), std::move(gsCopy), this, newChildIndex)));
			actionToChildIndex.push_back(newChildIndex);
			return children[newChildIndex].get();
		}
	}

	double AbstractMCTSNode::normalize(const double aValue, const double aMin, const double aMax)
	{
		if (aMin < aMax)
			return (aValue - aMin) / (aMax - aMin);

		// if bounds are invalid, then return same value
		return aValue;
	}

	double AbstractMCTSNode::noise(const double input, const double epsilon, const double random)
	{
		return (input + epsilon) * (1.0 + epsilon * (random - 0.5));
	}

	AbstractMCTSNode* AbstractMCTSNode::uct(AbstractMCTSParameters& params, std::mt19937& randomGenerator)
	{
		const bool iAmMoving = (gameState.currentPlayer == params.PLAYER_ID);

		std::vector<double> childValues(children.size(), 0);

		for (size_t i = 0; i < children.size(); ++i)
		{
			AbstractMCTSNode* child = children[i].get();

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
			AbstractMCTSNode* root = this;
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

	double AbstractMCTSNode::rollOut(TBSForwardModel& forwardModel, AbstractMCTSParameters& params, std::mt19937& randomGenerator)
	{
		if (params.ROLLOUTS_ENABLED) {
			auto gsCopy(gameState);
			int thisDepth = nodeDepth;

			while (!(rolloutFinished(gsCopy, thisDepth, params) || gsCopy.isGameOver)) {
				auto actions = forwardModel.generateActions(gsCopy);
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

	bool AbstractMCTSNode::rolloutFinished(GameState& rollerState, int depth, AbstractMCTSParameters& params)
	{
		if (depth >= params.ROLLOUT_LENGTH)      //rollout end condition.
			return true;

		//end of game
		return rollerState.isGameOver;
	}

	void AbstractMCTSNode::applyActionToGameState(TBSForwardModel& forwardModel, GameState& gameState, Action& action, AbstractMCTSParameters& params) const
	{
		params.REMAINING_FM_CALLS--;
		forwardModel.advanceGameState(gameState, action);
		while (gameState.currentPlayer != params.PLAYER_ID && !gameState.isGameOver)
		{
			if (params.opponentModel) // use default opponentModel to choose actions until the turn has ended
			{
				params.REMAINING_FM_CALLS--;
				auto actionSpace = forwardModel.generateActions(gameState);
				auto opAction = params.opponentModel->getAction(gameState, actionSpace);
				forwardModel.advanceGameState(gameState, opAction);
			}
			else // skip opponent turn
			{
				forwardModel.advanceGameState(gameState, Action::createEndAction(gameState.currentPlayer));
			}
		}
	}

	void AbstractMCTSNode::backUp(AbstractMCTSNode* node, const double result)
	{
		AbstractMCTSNode* n = node;
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

	int AbstractMCTSNode::mostVisitedAction(AbstractMCTSParameters& params, std::mt19937& randomGenerator)
	{
		int selected = -1;
		double bestValue = -std::numeric_limits<double>::max();
		bool allEqual = true;
		double first = -1;

		// find the most visited child state, for a small simulation count this value may be biased,
		// due to multiple actions yielding the same state
		std::vector<int> mostVisitedStates;
		int highestVisitCount = -1;
		for (size_t i = 0; i < children.size(); i++) {

			if (children[i] != nullptr)
			{
				if (children[i]->nVisits > highestVisitCount) {
					mostVisitedStates.clear();
					mostVisitedStates.push_back(i);
				}
				else if (children[i]->nVisits == highestVisitCount)
				{
					mostVisitedStates.push_back(i);
				}
					
				//childValue = noise(childValue, params.EPSILON, params.doubleDistribution_(randomGenerator));     //break ties randomly
			}
		}

		if (mostVisitedStates.empty()) 
		{
			// none of the child states has ever been visisted, return the first action
			return 0;
		}
		if (mostVisitedStates.size() == 1) 
		{
			// there is a single best state, but we still need to choose a random action yielding this state
			int targetState = mostVisitedStates[0];
			std::vector<int> candidate_actions;
			for (int i = 0; i < actionToChildIndex.size(); i++)
			{
				if (actionToChildIndex[i] == targetState) {
					candidate_actions.push_back(i);
				}
			}
			auto randDist = std::uniform_int_distribution<int>(0, candidate_actions.size()-1);
			int randomIndex = randDist(randomGenerator);
			return candidate_actions[randomIndex];
		}
		else
		{
			//If multiple states have the same visit count, we search for the state with the highest UCT value instead
			// before we sample an action that yields the resulting state
			return getActionOfHighestRatedState(params, randomGenerator);
		}
	}

	int AbstractMCTSNode::getActionOfHighestRatedState(AbstractMCTSParameters& params, std::mt19937& randomGenerator)
	{
		int bestState = -1;
		double bestValue = -std::numeric_limits<double>::max();

		for (size_t i = 0; i < children.size(); i++) {

			if (children[i] != nullptr) {
				double childValue = children[i]->value / (children[i]->nVisits + params.EPSILON);
				childValue = noise(childValue, params.EPSILON, params.doubleDistribution_(randomGenerator));     //break ties randomly
				if (childValue > bestValue) {
					bestValue = childValue;
					bestState = i;
				}
			}
		}

		if (bestState == -1)
		{
			// just for safety, but this case should never occure since it requires the state to be worse than the numerical minimum
			return 0;
		}
		else {
			// there is a single best state, but we still need to choose a random action yielding this state
			int targetState = bestState;
			std::vector<int> candidate_actions;
			for (int i = 0; i < actionToChildIndex.size(); i++)
			{
				if (actionToChildIndex[i] == targetState) {
					candidate_actions.push_back(i);
				}
			}
			auto randDist = std::uniform_int_distribution<int>(0, candidate_actions.size());
			int randomIndex = randDist(randomGenerator);
			return candidate_actions[randomIndex];
		}
	}

	void AbstractMCTSNode::print() const
	{
		std::cout << this->value / this->nVisits << "; " << this->nVisits << "; " << children.size();
	}

}
