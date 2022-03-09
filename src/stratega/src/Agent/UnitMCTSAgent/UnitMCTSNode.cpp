#include <Stratega/Agent/UnitMCTSAgent/UnitMCTSNode.h>
#include <utility>

namespace SGA
{
	UnitMCTSNode::UnitMCTSNode(ForwardModel& forwardModel, GameState newGameState, std::vector<int> unitIndex_, int unitThisStep_, int newPlayerID, int nodeID_) :
		ITreeNode<SGA::UnitMCTSNode>(forwardModel, std::move(newGameState), newPlayerID)
	{
		nodeID = nodeID_;
		if (newGameState.isGameOver()) {
			std::cout << "Initiating the node, Found: Game Over" << std::endl;
			return;
		}
		unitIndex = unitIndex_;
		unitThisStep = unitThisStep_;

		// initialize the actionSpace

		int tmp_unitThisStep = unitThisStep_;

		// should never show up
		if (tmp_unitThisStep == static_cast<int>(unitIndex_.size())) {
			std::cout << "Initiating the node, Found: tmp_unitThisStep == unitIndex_.size()" << std::endl;

		}


		Entity* unit = nullptr;
		auto allUnits = this->gameState.getPlayerEntities(newPlayerID);
		//std::cout << "Entity length: " << allUnits.size() << std::endl;

		while (true) {
			//auto* 
			unit = this->gameState.getEntity(unitIndex[tmp_unitThisStep]);
			if (unit == nullptr) {
				tmp_unitThisStep++;

				// happens when all units are killed, should not happen
				// could be met when the state is terminated? 8_21
				if (tmp_unitThisStep == static_cast<int>(unitIndex_.size())) {
					std::cout << "All Unit killed init1" << std::endl;
					return; // 8_21
				}
			}
			else {
				break;
			}
		}

		actionSpace = forwardModel.generateUnitActions(this->gameState, *unit, playerID, false);
		//std::cout << "Action Len init1: " << actionSpace.size() << std::endl;
		unitThisStep = tmp_unitThisStep;

		actionHashes = std::map<int, int>();
		actionToReward = std::map<int, double>();
		stateCounter = std::map<int, int>();
		actionHashVector = std::vector<int>();
		nextStateHashVector = std::vector<int>();
	}

	UnitMCTSNode::UnitMCTSNode(ForwardModel& forwardModel, GameState newGameState, UnitMCTSNode* parent,
				const int newChildIndex, std::vector<int> unitIndex_, int unitThisStep_, int newPlayerID, int nodeID_) :
		ITreeNode<SGA::UnitMCTSNode>(forwardModel, std::move(newGameState), parent, newChildIndex, newPlayerID)
	{
		unitIndex = unitIndex_;
		nodeID = nodeID_;

		int tmp_unitThisStep = unitThisStep_;
		if (tmp_unitThisStep == static_cast<int>(unitIndex_.size())) {
			std::cout << "Initiating the node, Found: tmp_unitThisStep == unitIndex_.size()" << std::endl;
		}

		// initialize the actionSpace

		Entity* unit = nullptr;
		while (true) {
			unit = this->gameState.getEntity(unitIndex[tmp_unitThisStep]);
			if (unit == nullptr) {
				tmp_unitThisStep++;

				// happens when all units are killed, should not happen
				if (tmp_unitThisStep == static_cast<int>(unitIndex_.size())) {
					std::cout << "All Unit killed init 2" << std::endl;
					return; //8_21
				}
			}
			else {
				break;
			}
		}

		actionSpace = forwardModel.generateUnitActions(this->gameState, *unit, playerID, false);
		unitThisStep = tmp_unitThisStep;

		actionHashes = std::map<int, int>();
		actionToReward = std::map<int, double>();
		stateCounter = std::map<int, int>();
		actionHashVector = std::vector<int>();
		nextStateHashVector = std::vector<int>();

	}

	double UnitMCTSNode::getValue(std::map<int, std::vector<double> >* absNodeToStatistics) {
		if (isAbstracted) {
			absValue = (*absNodeToStatistics)[absNodeID][0];
			return absValue;
		}
		else {
			return value;
		}
	}

	void UnitMCTSNode::setValue(double result, std::map<int, std::vector<double> >* absNodeToStatistics, bool increase) {
		if (isAbstracted) {
			if (increase) {
				(*absNodeToStatistics)[absNodeID][0] += result;
			}
			else {
				(*absNodeToStatistics)[absNodeID][0] = result;
			}
		}
		/*else {
			if (increase) {
				value += result;
			}
			else {
				value = result;
			}
		}
		*/
		// also modify the original 
		if (increase) {
			value += result;
		}
		else {
			value = result;
		}
	}

	void UnitMCTSNode::eliminateAbstraction()
	{
		if (isAbstracted) {
			isAbstracted = false;
		}
		for (int i = 0; i < childExpanded; i++)
		{
			children[i]->eliminateAbstraction();
		}
	}


	int UnitMCTSNode::getVisitCount(std::map<int, std::vector<double> >* absNodeToStatistics) {
		if (isAbstracted) {
			absVisitCount = static_cast<int>((*absNodeToStatistics)[absNodeID][1]);
			return absVisitCount;
		}
		else {
			return nVisits;
		}
	}

	void UnitMCTSNode::setVisitCount(double result, std::map<int, std::vector<double> >* absNodeToStatistics, bool increase) {
		if (isAbstracted) {
			if (increase) {
				(*absNodeToStatistics)[absNodeID][1] += result;
			}
			else {
				(*absNodeToStatistics)[absNodeID][1] = result;
			}

		}
		/*else {
			if (increase) {
				nVisits += result;
			}
			else {
				nVisits = result;
			}
		}*/
		if (increase) {
			nVisits += static_cast<int>(result);
		}
		else {
			nVisits = static_cast<int>(result);
		}
	}


	/// <summary>
	/// setting up basic node statistics
	/// </summary>to
	void UnitMCTSNode::initializeNode()
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

	// set the depth for one child
	void UnitMCTSNode::setDepth(int depth)
	{
		nodeDepth = depth;
		/*for (size_t i = 0; i < this->children.size(); i++) {
			children.at(i)->setDepth(depth + 1);
		}
		*/
	}

	void UnitMCTSNode::increaseTreeSize()
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


	/// <summary>
	/// Start MCTS with the provided parameters
	/// </summary>
	/// <param name="params">parameters of the search</param>
	/// <param name="randomGenerator"></param>
	void UnitMCTSNode::searchMCTS(ForwardModel& forwardModel, UnitMCTSParameters& params,
		boost::mt19937& randomGenerator, std::map<int, std::vector<UnitMCTSNode*> >* depthToNodes,
		std::map<int, std::vector<double> >* absNodeToStatistics) {

		int numIterations = 0;
		bool stop = false;
		//int prevCallCount = static_cast<int>(params.REMAINING_FM_CALLS);

		// stop in case the set number of fmCalls has been reached
		while (!stop) {
			UnitMCTSNode* selected = treePolicy(forwardModel, params, randomGenerator, depthToNodes, absNodeToStatistics);

			if (selected == nullptr) continue;

			// DEBUG
			if (selected == nullptr) std::cout << "Returns an empty pointer" << std::endl;

			double deltaAll = 0.0;
			double d_tmp = 0.0;
			int Nrollout = 1;

			for (int i = 0; i < Nrollout; i++) {
				d_tmp = selected->rollOut(forwardModel, params, randomGenerator);
				deltaAll += d_tmp;
			}

			const double delta = deltaAll / Nrollout;  //cout << "delta: " << delta << "\n";

			backUp(selected, delta, absNodeToStatistics);
			numIterations++;
			n_search_iteration++;
			//std::cout << " After one search: " <<  params.REMAINING_FM_CALLS <<std::endl;


			if (params.DO_STATE_ABSTRACTION) {
				stop = params.REMAINING_FM_CALLS <= 0 || numIterations % params.batch_size == 0;
			}
			else {
				stop = params.REMAINING_FM_CALLS <= 0 || numIterations == params.maxFMCalls;
			}

			//prevCallCount = params.REMAINING_FM_CALLS;
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
	UnitMCTSNode* UnitMCTSNode::treePolicy(ForwardModel& forwardModel, UnitMCTSParameters& params, boost::mt19937& randomGenerator,
		std::map<int, std::vector<UnitMCTSNode*> >* depthToNodes, std::map<int, std::vector<double> >* absNodeToStatistics)
	{
		UnitMCTSNode* cur = this;

		while (!(cur->gameState.isGameOver()))// && cur->nodeDepth < params.ROLLOUT_LENGTH)
		{

			if (!cur->isFullyExpanded()) {
				return (cur->expand(forwardModel, params, randomGenerator, depthToNodes));

			}
			else {
				// meet a leaf child, return
				if (static_cast<int>(cur->children.size()) == 0)
				{
					return cur;
				}

				cur = cur->uct(params, randomGenerator, absNodeToStatistics);
			}
		}
		return cur;
	}

	UnitMCTSNode* UnitMCTSNode::expand(ForwardModel& forwardModel, UnitMCTSParameters& params, boost::mt19937& /*randomGenerator*/,
		std::map<int, std::vector<UnitMCTSNode*> >* depthToNodes)
	{
		//todo remove unnecessary copy of gameState
		auto gsCopy(gameState);
		int new_childIndex = childExpanded;

		int isTurnEnd = applyActionToGameState(forwardModel, gsCopy, actionSpace.at(new_childIndex), params);

		//std::cout << " [expand state hash]: " << unitStateHash(forwardModel, gameState, *gameState.getEntity(unitIndex[unitThisStep])) << std::endl;

		// decide which is the unit for the next depth
		int unitNextStep = 0;
		if (isTurnEnd)
		{
			unitNextStep = 0;
		}
		else { //the entity won't be dead
			auto actionSpace_tmp =
				forwardModel.generateUnitActions(gsCopy, *gsCopy.getEntity(unitIndex[unitThisStep]), params.PLAYER_ID, false);
			if (static_cast<int>(actionSpace_tmp.size()) == 0) {
				unitNextStep = unitThisStep + 1;
			}
			else {
				unitNextStep = unitThisStep;
			}

		}

		// find next alived unit
		if (!isTurnEnd) {
			for (; unitNextStep < static_cast<int>(unitIndex.size()); unitNextStep++) {
				auto* u = gsCopy.getEntity(unitIndex[unitNextStep]);
				if (u != nullptr) {
					break;
				}
			}
		}

		// all units moved, refresh the moving order by ending this turn, this could raize terminate state
		if (unitNextStep == static_cast<int>(unitIndex.size()))
		{
			auto endA = Action::createEndAction(params.PLAYER_ID);
			applyActionToGameState(forwardModel, gsCopy, endA, params);
			unitNextStep = 0;
		}

		// TODO@homomorphism collecting transitions
		// addTransition(int stateHash, int actionHash, int nextStateHash, double reward);
		double reward = params.heuristic.get()->evaluateGameState(forwardModel, gsCopy, params.PLAYER_ID);
		int eID = unitIndex[unitThisStep];
		int state_hash = SGA::unitStateHash(forwardModel, gameState, *gameState.getEntity(eID));
		auto unit_nextState = gameState.getEntity(eID);
		int next_state_hash = -1;
		if (unit_nextState != nullptr) // this unit could be dead next step
		{
			next_state_hash = SGA::unitStateHash(forwardModel, gsCopy, *unit_nextState);
		}

		int action_hash = 0;
		params.global_transition.addTransition(state_hash, action_hash, next_state_hash, reward);
		actionToReward.insert(std::pair<int, double>(action_hash, reward));
		stateCounter.insert(std::pair<int, int>(next_state_hash, 1));
		actionHashes.insert(std::pair<int, int>(action_hash, 1));
		actionHashVector.push_back(action_hash);
		nextStateHashVector.push_back(next_state_hash);

		children.push_back(std::unique_ptr<UnitMCTSNode>(new UnitMCTSNode(forwardModel, std::move(gsCopy), this, new_childIndex,
			unitIndex, unitNextStep, params.PLAYER_ID, params.global_nodeID)));
		params.global_nodeID++;
		children[children.size() - static_cast<size_t>(1)]->setDepth(nodeDepth + 1);
		if (nodeDepth + 1 > params.maxDepth)
		{
			params.maxDepth = nodeDepth + 1;
		}
		childExpanded += 1;

		if (!(depthToNodes->count(nodeDepth + 1))) {
			depthToNodes->insert(std::pair<int, std::vector<UnitMCTSNode*> >(nodeDepth + 1, std::vector<UnitMCTSNode*>()));
		}
		((*depthToNodes)[nodeDepth + 1]).push_back(children[new_childIndex].get());


		return children[new_childIndex].get();
	}

	double UnitMCTSNode::normalize(const double aValue, const double aMin, const double aMax)
	{
		if (aMin < aMax)
			return (aValue - aMin) / (aMax - aMin);

		// if bounds are invalid, then return same value
		return aValue;
	}

	// one case is used in to choose the best action for breaing a equal distribution
	double UnitMCTSNode::noise(const double input, const double epsilon, const double random)
	{
		return (input + epsilon) * (1.0 + epsilon * (random - 0.5));
	}

	UnitMCTSNode* UnitMCTSNode::uct(UnitMCTSParameters& params, boost::mt19937& randomGenerator, std::map<int, std::vector<double> >* absNodeToStatistics)
	{
		const bool iAmMoving = (gameState.getCurrentTBSPlayer() == params.PLAYER_ID);

		std::vector<double> childValues(children.size(), 0);

		for (size_t i = 0; i < children.size(); ++i)
		{
			UnitMCTSNode* child = children[i].get();

			const double hvVal = child->getValue(absNodeToStatistics); //child->value;

			//double childValue = hvVal / (child->nVisits + params.EPSILON);
			double childValue = hvVal / (child->getVisitCount(absNodeToStatistics) + params.EPSILON);

			//childValue = normalize(childValue, bounds[0], bounds[1]); //childValue = normalize(childValue, -200, 200);

			//double uctValue = childValue +
			//	params.K * sqrt(log(this->nVisits + 1) / (child->nVisits + params.EPSILON));

			double uctValue = childValue +
				params.K * sqrt(log(this->getVisitCount(absNodeToStatistics) + 1) / (child->getVisitCount(absNodeToStatistics) + params.EPSILON));

			uctValue = noise(uctValue, params.EPSILON, params.doubleDistribution_(randomGenerator));     //break ties randomly
			childValues[i] = uctValue;
		}

		int which = -1;
		double bestValue = iAmMoving ? -std::numeric_limits<double>::max() : std::numeric_limits<double>::max();

		for (size_t i = 0; i < children.size(); ++i) {
			if ((iAmMoving && childValues[i] > bestValue) || (!iAmMoving && childValues[i] < bestValue)) {
				which = static_cast<int>(i);
				bestValue = childValues[i];
			}
		}

		if (which == -1)
		{
			std::cout << "this subtree:" << "\n";
			printTree();

			std::cout << "\n\n" << "the whole tree:" << "\n";
			UnitMCTSNode* root = this;
			while (root->parentNode != nullptr)
			{
				root = root->parentNode;
			}
			printTree("", root, true, "root");
			std::cout << "\n\n";

			//if(this.children.length == 0)
			std::cout << "Warning! couldn't find the best UCT value " << which << " : " << children.size() << "\n";
			std::cout << "Children Size: " << children.size() << std::endl;
			std::cout << nodeDepth << ", AmIMoving? " << iAmMoving << "\n";

			for (size_t i = 0; i < children.size(); ++i)
				std::cout << "\t" << childValues[i] << "\n";
			std::cout << "; selected: " << which << "\n";
			std::cout << "; isFullyExpanded: " << isFullyExpanded() << "\n";
			boost::random::uniform_int_distribution<size_t> distrib(0, children.size() - 1);

			which = static_cast<int>(distrib(randomGenerator));
		}

		//std::cout << which << " children size " << children.size() << std::endl;
		return children[which].get();
	}

	double UnitMCTSNode::rollOut(ForwardModel& forwardModel, UnitMCTSParameters& params, boost::mt19937& randomGenerator)
	{
		if (params.ROLLOUTS_ENABLED) {
			auto gsCopy(gameState);
			//int rolloutDepth = 0;//nodeDepth;
			int rolloutDepth = nodeDepth;

			while (!(rolloutFinished(gsCopy, rolloutDepth, params) || gsCopy.isGameOver())) {
				auto actions = forwardModel.generateActions(gsCopy, params.PLAYER_ID);
				if (static_cast<int>(actions.size()) == 0)
					break;
				boost::random::uniform_int_distribution<size_t> randomDistribution(0, static_cast<int>(actions.size()) - 1);
				applyActionToGameState(forwardModel, gsCopy, actions.at(randomDistribution(randomGenerator)), params);
				rolloutDepth++;
			}
			//return normalize(params.STATE_HEURISTIC->evaluateGameState(forwardModel, gsCopy, params.PLAYER_ID), -200, 200);
			return params.heuristic->evaluateGameState(forwardModel, gsCopy, params.PLAYER_ID);
		}

		//return normalize(params.STATE_HEURISTIC->evaluateGameState(forwardModel, gameState, params.PLAYER_ID), -1000, 1000);
		return params.heuristic->evaluateGameState(forwardModel, gameState, params.PLAYER_ID);
	}

	bool UnitMCTSNode::rolloutFinished(GameState& rollerState, int depth, UnitMCTSParameters& params)
	{
		if (depth >= params.ROLLOUT_LENGTH)      //rollout end condition.
			return true;

		//end of game
		return rollerState.isGameOver();
	}


	// return isTurnEnd == true when an opponent turn run in the applyActionToGameState
	int UnitMCTSNode::applyActionToGameState(ForwardModel& forwardModel, GameState& targetGameState, Action& action, UnitMCTSParameters& params) const
	{
		params.REMAINING_FM_CALLS--;
		forwardModel.advanceGameState(targetGameState, action);
		bool isTurnEnd = false;
		while (targetGameState.getCurrentTBSPlayer() != params.PLAYER_ID && !targetGameState.isGameOver())
		{

			isTurnEnd = true;
			if (params.OPPONENT_MODEL) // use default opponentModel to choose actions until the turn has ended
			{
				params.REMAINING_FM_CALLS--;
				auto actions = forwardModel.generateActions(targetGameState, params.PLAYER_ID);
				auto opAction = params.OPPONENT_MODEL->getAction(targetGameState, actions, params.PLAYER_ID);
				forwardModel.advanceGameState(targetGameState, opAction);
			}
			else // skip opponent turn, opponent ends the turn
			{
				forwardModel.advanceGameState(targetGameState, Action::createEndAction(targetGameState.getCurrentTBSPlayer()));
			}
		}
		return isTurnEnd;
	}

	void UnitMCTSNode::backUp(UnitMCTSNode* node, const double result, std::map<int, std::vector<double> >* absNodeToStatistics)
	{
		UnitMCTSNode* n = node;
		while (n != nullptr)
		{
			n->setVisitCount(1, absNodeToStatistics, true);
			//n->nVisits++;
			n->setValue(result, absNodeToStatistics, true);
			//n->value += result;
			if (result < n->bounds[0]) {
				n->bounds[0] = result;
			}
			if (result > n->bounds[1]) {
				n->bounds[1] = result;
			}
			n = n->parentNode;
		}
	}

	int UnitMCTSNode::mostVisitedAction(UnitMCTSParameters& params, boost::mt19937& randomGenerator)
	{
		int selected = -1;
		double bestValue = -std::numeric_limits<double>::max();
		bool allEqual = true;
		double first = -1;

		//cout << "Remaining budget: " << params.REMAINING_FM_CALLS << "\n";
		//printTree();

		std::vector<double> childValues(static_cast<int>(children.size()), 0);
		for (size_t i = 0; i < children.size(); ++i)
		{
			UnitMCTSNode* child = children[i].get();

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

				childValue = noise(childValue, params.EPSILON, params.doubleDistribution_(randomGenerator));     //break ties randomly

				if (childValue > bestValue) {
					bestValue = childValue;
					//selected = static_cast<int>(i);
					selected = static_cast<int>(children[i]->childIndex);
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

	int UnitMCTSNode::bestAction(UnitMCTSParameters& params, boost::mt19937& randomGenerator)
	{
		int selected = -1;
		double bestValue = -std::numeric_limits<double>::max();

		for (size_t i = 0; i < children.size(); i++) {

			if (children[i] != nullptr) {
				double childValue = children[i]->value / (children[i]->nVisits + params.EPSILON);
				childValue = noise(childValue, params.EPSILON, params.doubleDistribution_(randomGenerator));     //break ties randomly
				if (childValue > bestValue) {
					bestValue = childValue;
					selected = children[i]->childIndex; //static_cast<int>(i);
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

	void UnitMCTSNode::print() const
	{

		std::cout << this->value / this->nVisits << "; " << this->nVisits << "; " << children.size() << " ";
		if (this->parentNode != nullptr)
		{
			auto a = this->parentNode->actionSpace[this->childIndex];
			this->parentNode->gameState.printActionInfo(a);
		}
	}

	void UnitMCTSNode::printActionInfo() const
	{

		if (this->parentNode != nullptr)
		{
			auto a = this->parentNode->actionSpace[this->childIndex];

			this->parentNode->gameState.printActionInfo(a);

		}
	}

	void UnitMCTSNode::get_branching_number(std::vector<int>* v, int* n)
	{
		*n += 1;
		//if (children.size() != 0)
		if (childExpanded != 0)
		{
			v->push_back(static_cast<int>(children.size()));
		}
		for (int i = 0; i < static_cast<int>(children.size()); i++)
		{
			children[i]->get_branching_number(v, n);
		}
	}

}
