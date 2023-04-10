#include <Stratega/Agent/NaiveMCTSAgent/NaiveMCTSNode.h>
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	NaiveMCTSNode::NaiveMCTSNode(
		ForwardModel& forwardModel, 
		GameState newGameState, 
		std::vector<int> actionCombination, int newOwnerID)
		:
		ITreeNode<SGA::NaiveMCTSNode>(forwardModel, std::move(newGameState), newOwnerID)
	{
		actionCombinationTook = actionCombination;
		childExpanded = 0;

		if(!gameState.isGameOver())
			initializeNode(forwardModel, newGameState, newOwnerID);
	}

	NaiveMCTSNode::NaiveMCTSNode(
		ForwardModel& forwardModel,
		GameState newGameState,
		NaiveMCTSNode* newParent,
		std::vector<int> actionCombination,
		const int newChildIndex,
		int newOwnerID)
		:
		ITreeNode<SGA::NaiveMCTSNode>(forwardModel, std::move(newGameState), newParent, newChildIndex, newOwnerID)
	{
		actionCombinationTook = actionCombination;
		computeActionSpace(forwardModel);

		if (!gameState.isGameOver())
			initializeNode(forwardModel, newGameState, newOwnerID);

	}

	/// <summary>
	/// setting up basic node statistics
	/// </summary>to
	void NaiveMCTSNode::initializeNode(ForwardModel& forwardModel,
		GameState newGameState,
		int newOwnerID)
	{
		if (parentNode) {
			nodeDepth = parentNode->nodeDepth + 1;
			increaseTreeSize();
		}
		else {
			nodeDepth = 0;
		}
		childExpanded = 0;

		auto allUnits = this->gameState.getPlayerEntities(newOwnerID);
		numUnit = allUnits.size();
		/*for (auto u : allUnits) {
			auto entityActionSpace =
				forwardModel.generateUnitActions(newGameState, u, newOwnerID, false);
			if (entityActionSpace.size() == 0){
				numUnit--;
			}
		}
		*/

		/* generate action space for each unit */
		int uid = 0;
		for (auto u : allUnits) {
			auto entityActionSpace =
				forwardModel.generateUnitActions(newGameState, u, newOwnerID, false);
			nodeActionSpace.push_back(std::vector<SGA::Action>{});
			if(entityActionSpace.size() == 0){
				entityActionSpaceSize[uid] = 0;
				uid++;
				continue;
			}
			entityActionSpaceSize[uid] = entityActionSpace.size();
			for (int i = 0; i < entityActionSpace.size(); i++) {
				nodeActionSpace[uid].push_back(entityActionSpace[i]);
			}
			uid++;
		}

		/* initialize counter and values */
		for (int i = 0; i < nodeActionSpace.size(); i++) {
			combinationCount.push_back(std::vector<int>(nodeActionSpace[i].size(), 0));
			combinationValue.push_back(std::vector<double>(nodeActionSpace[i].size(), 0.0));
		}

	}

	/// <summary>
	/// Start MCTS with the provided parameters
	/// </summary>
	/// <param name="params">parameters of the search</param>
	/// <param name="randomGenerator"></param>
	void NaiveMCTSNode::searchMCTS(ForwardModel& forwardModel, NaiveMCTSParameters& params, boost::mt19937& randomGenerator) {
		//std::cout << "searching\n";
		NaiveMCTSNode* last_selected = nullptr;
		int n_repeat_selection = 0;
		int tmp_fm_call_spent = 0;
		int fm_call_not_advance = 0;
		while (!params.isBudgetOver()) {

			NaiveMCTSNode* selected = treePolicy(forwardModel, params, randomGenerator);
			//std::cout << "finished treePolicy\n";
			if (last_selected != nullptr) {
				if (last_selected == selected)
					n_repeat_selection++;
				else
					n_repeat_selection = 0;
			}
			last_selected = selected;
			tmp_fm_call_spent = params.currentFMCalls;
			//std::cout << "starting rollout\n";
			int n_rollout = 1;
			double delta = 0.0;
			for (int i = 0; i < n_rollout; i++){
				delta += selected->rollOut(forwardModel, params, randomGenerator);
				//delta += params.heuristic->evaluateGameState(forwardModel, selected->gameState, playerID);
			}
			delta /= double(n_rollout);
			//std::cout << "finished rollout: " << delta <<"\n";

			/*rollout does not spent forward model, if consecutively not spending any forward model calls
			* an infinite search will appear
			*/
			if (params.budgetType == Budget::FMCALLS) {
				if (params.currentFMCalls == tmp_fm_call_spent) {
					// std::cout << "depth: " << selected->nodeDepth << "\n";
					fm_call_not_advance++;
					if (fm_call_not_advance > 20)break;
				}
				else {
					fm_call_not_advance = 0;
				}
			}

			//std::cout << "starting backUp\n";
			backUp(selected, delta, params);
			//std::cout << "finished backUp\n";
			
			params.currentIterations++;
			//if (n_repeat_selection >= 20) // repeated selection
			//	break;
			if (n_repeat_selection >= 100)
				break;
			if (params.currentIterations > 1000)
				break;
			//std::cout<< "n_repeat_selection: "<< n_repeat_selection<< " fm calls: "<< params.currentFMCalls << "\n";
		}
		//std::cout<<"Is budget over? " << params.isBudgetOver() << "; Cost FM calls: "<< params.currentFMCalls << "; Iterations: " << params.currentIterations <<
		//	"; repeat: " << n_repeat_selection << "\n";
	}

	/// <summary>
	/// Select the node to be expanded next.
	/// Apply UCT until a node has been found that is not fully expanded yet
	/// or the maximum depth has been reached.
	/// </summary>
	/// <param name="params">parameters of the search</param>
	/// <param name="randomGenerator"></param>
	/// <returns></returns>
	NaiveMCTSNode* NaiveMCTSNode::treePolicy(ForwardModel& forwardModel, NaiveMCTSParameters& params, boost::mt19937& randomGenerator)
	{
		//std::cout << "treePolicy\n";
		NaiveMCTSNode* cur = this;

		while (!cur->gameState.isGameOver() && cur->nodeDepth < params.maxDepth)//cur->nodeDepth < params.rolloutLength)
		{
			//std::cout << "continue uct\n";
			cur = cur->uct(params, randomGenerator, forwardModel);
		}
		//std::cout << "closing tree policy\n";
		return cur;
	}

	NaiveMCTSNode* NaiveMCTSNode::expand(ForwardModel& forwardModel, NaiveMCTSParameters& params, boost::mt19937& /*randomGenerator*/)
	{
		/* deprecated */
		auto gsCopy(gameState);
		auto action = actionSpace.at(children.size());
		applyActionToGameState(forwardModel, gsCopy, action, params, playerID);

		// generate child node and add it to the tree
		children.push_back(std::unique_ptr< NaiveMCTSNode >(new NaiveMCTSNode(
			forwardModel, std::move(gsCopy), this, std::vector<int>(), childExpanded, this->ownerID)));
		//std::cout<< children.size() << " childExapnded: "<< childExpanded<<std::endl;
		childExpanded++;

		return children[static_cast<size_t>(children.size() - 1)].get();
	}


	NaiveMCTSNode* NaiveMCTSNode::uct(NaiveMCTSParameters& params,
		boost::mt19937& randomGenerator,
		ForwardModel& forwardModel)
	{
		if (gameState.isGameOver()) 
			return this;
		
		if (nodeDepth == params.maxDepth)
			return this;
		//std::cout << "1\n";

		double epsilonZeroCompetitor = params.doubleDistribution_(randomGenerator);
		if (epsilonZeroCompetitor < params.epsilon_0 || children.size() == 0) {
			/*exploration for pi_0 -> call pi_l
			* construct action combination by selecting each unit action independently
			*/
			double epsilonLCompetitor = params.doubleDistribution_(randomGenerator);

			std::vector<int>  actionCombinationTook = {};
			
			/*select unit actions*/
			for (int i = 0; i < numUnit; i++) {
				double epsilonLCompetitor = params.doubleDistribution_(randomGenerator);
				int which = -1;
				if(entityActionSpaceSize[i]==0){
					actionCombinationTook.push_back(which);
					continue;
				}
				if (epsilonLCompetitor < params.epsilon_l) {
					// exploration for pi_l
					// uniformly select a unit action (X_i)
					boost::random::uniform_int_distribution< size_t > distrib(0, entityActionSpaceSize[i]-1);
					which = static_cast<int>(distrib(randomGenerator));
				}
				else {
					//std::cout << "5\n";
					// select highest-value X_i (unit Action)
					which = std::distance(combinationValue[i].begin(),
						std::max_element(combinationValue[i].begin(), combinationValue[i].end()));
				}

				//std::cout << "6\n";
				actionCombinationTook.push_back(which);

				// counter for this value ++
				combinationCount[i][which]++;
			}
			//std::cout << "6..66\n";
			std::string newChildID = params.intVectorToString(actionCombinationTook);
			//std::cout << "-----------> new child ID: " << newChildID << "\n";
			if (combinationToChildID.find(newChildID) != combinationToChildID.end()) {
				/*constructed combination existed as a child, return this child*/

				//std::cout << "constructed combination existed as a child, return this child\n";
				auto child = children[combinationToChildID[newChildID]].get();

				childrenCount[combinationToChildID[newChildID]]++;
				//std::cout << "returning child\n";
				return child; //->uct(params, randomGenerator, forwardModel);
			}
			else {
				/* constructed action combination does not exist, add the new child and return it
				*/

				//std::cout << "8\n";
				auto gsCopy(gameState);

				// create a new states for this child by executing all actions
				bool hasEndTurn = false;
				for (int uid = 0; uid < actionCombinationTook.size(); uid++) {
					//std::cout << "8.1, nodeActionSpace.size(): " << nodeActionSpace.size() << "\n";
					//std::cout << "nodeActionSpace[uid].size(): " << nodeActionSpace[uid].size() << "\n";

					//std::cout << "uid: " << uid <<" , actionCombination[uid]: " << actionCombination[uid] << "\n";
					if(nodeActionSpace[uid].size() == 0){
						actionCombinationTook[uid] = -1;
						continue;
					}
					auto action = nodeActionSpace[uid][actionCombinationTook[uid]];

					// TODO check action whether valid
					if( action.validate(gsCopy) ){
						applyActionToGameState(forwardModel, gsCopy, action, params, playerID);
					}

					/*do not execute multiple End action to avoid round-skipping*/
					/*if (action.getActionFlag() == ActionFlag::EndTickAction) {
						//std::cout << "there is a situation of round skipping during search\n";
						if(hasEndTurn){
							break;
						}
						hasEndTurn = true;
					}
					*/
				}

				auto actionSpace = forwardModel.generateActions(gsCopy, playerID);

				// skip End action
				if(actionSpace.size() == 1){
					applyActionToGameState(forwardModel, gsCopy, actionSpace[0], params, playerID);
				}
				//std::cout << "8.5, is gsCopy terminated?: " << gsCopy.isGameOver() <<" \n";

				children.push_back(std::unique_ptr< NaiveMCTSNode >(new NaiveMCTSNode(
					forwardModel, std::move(gsCopy), this, actionCombinationTook, childExpanded, this->ownerID)));
				children[children.size() - 1]->nodeDepth = nodeDepth + 1;
				if(nodeDepth+1 == params.maxDepth){
					children[children.size() - 1]->isBottom = true;
				}
				childrenValue.push_back(0.0);
				childrenCount.push_back(1.0);

				combinationToChildID.insert(std::pair<std::string, int>{newChildID, children.size() - 1});
				//std::cout << "9\n";
				return children[children.size()-1].get();
			}

		}
		else {
			/*exploitation for pi_0 -> call pi_g
			* select one of the current child
			* if children.size() == 0, this does not work
			*/

			//std::cout << "10\n";
			double epsilonGCompetitor = params.doubleDistribution_(randomGenerator);
			int which = -1;
			if (epsilonGCompetitor < params.epsilon_g) {
				/*exploration for pi_g
				* randomly select a current child
				*/

				//std::cout << "11\n";
				boost::random::uniform_int_distribution< size_t > distrib(0, children.size() - 1);
				which = static_cast<int>(distrib(randomGenerator));
			}
			else {
				//std::cout << "12\n";
				// exploitation for pi_g
				// select the highest-value node
				childrenValue; // add some noise
				which = std::distance(childrenValue.begin(),
					std::max_element(childrenValue.begin(), childrenValue.end()));

			}
			//std::cout << "13, which: " << which << "\n";
			//std::cout << "childrenCount.size(): " << childrenCount.size() << "\n";
			childrenCount[static_cast<size_t>(which)] ++;
			//TODO combination count
			auto comb = children[static_cast<size_t>(which)]->actionCombinationTook;

			for(int i=0 ; i< combinationCount.size() ; i++)
			{
				if(comb[i] == -1){
					continue;
				}
				//std::cout << "i: " << i << " combinationCount[i].size(): " << combinationCount[i].size() << "\n";
				combinationCount[i][comb[i]]++;
			}

			//std::cout << "14\n";
			return children[static_cast<size_t>(which)]->uct(params, randomGenerator, forwardModel);
		}

		//return children[static_cast<size_t>(which)].get();
	}

	//Executes the rollout phase of MCTS
	double NaiveMCTSNode::rollOut(ForwardModel& forwardModel, NaiveMCTSParameters& params, boost::mt19937& randomGenerator)
	{
		//... only if rollouts are enabled in the parameter settings.
		if (params.rolloutsEnabled) {

			//Create a copy and mark our depth on the tree.
			auto gsCopy(gameState);
			int thisDepth = nodeDepth;

			// [BUGfixed] for FMCALLS budget: selected state is a terminal state that cost no budget
			if (gsCopy.isGameOver() && params.budgetType == Budget::FMCALLS) {
				//std::cout<<"catch terminal state!\n";
				params.currentFMCalls++;
			}

			if(thisDepth == params.rolloutLength){
				/*
				auto candidate_node = this;
				while(candidate_node->parentNode != nullptr){
					candidate_node->printActionTookInfo();
					candidate_node = candidate_node->parentNode;
					std::cout << "\nparent:\n";
				}
				std::cout << "End pringting uct trajectory actions\n";
				system("pause");
				//*/
				for (int i = 0; i < params.rolloutLength; i++){
					auto actions = forwardModel.generateActions(gsCopy, params.PLAYER_ID);
					if (actions.size() == 0)
						return params.heuristic->evaluateGameState(forwardModel, gsCopy, params.PLAYER_ID);

					//Pick one action at random and apply it to the current game state. 
					boost::random::uniform_int_distribution<size_t> randomDistribution(0, actions.size() - 1);
					applyActionToGameState(forwardModel, gsCopy, actions.at(randomDistribution(randomGenerator)), params, playerID);
					thisDepth++;
				}
				return params.heuristic->evaluateGameState(forwardModel, gsCopy, params.PLAYER_ID);
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

	bool NaiveMCTSNode::rolloutFinished(GameState& rollerState, int depth, NaiveMCTSParameters& params)
	{
		if (depth >= params.rolloutLength) {
			//std::cout<<"depth out of range\n";
			return true;
		}

		return rollerState.isGameOver();
	}

	void NaiveMCTSNode::applyActionToGameState(
		ForwardModel& forwardModel,
		GameState& targetGameState,
		Action& action,
		NaiveMCTSParameters& params,
		int /*playerID*/) const
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

	void NaiveMCTSNode::updateValue(std::vector<int> actionCombinationTook, const double result, NaiveMCTSParameters& params){
		std::string comb_str = params.intVectorToString(actionCombinationTook);
		//std::cout << "comb_str: " << comb_str << "\n";
		//std::cout << "combinationToChildID.size(): " << combinationToChildID.size() << "\n";
		int child_id = combinationToChildID[comb_str];
		//std::cout << "actionCombinationTook:\n";
		//for(int i =0;i< actionCombinationTook.size();i++){
		//	std::cout << actionCombinationTook[i] << "  ";
		//}
		//std::cout << "\ncombinationValue.size(): "<< combinationValue.size()<<"\n";

		// update X_i
		for(int i = 0 ; i< combinationValue.size(); i++){
			if(actionCombinationTook[i] == -1){
				continue;
			}
			//std::cout << "combinationValue["<<i<<"].size(): " << combinationValue[i].size() << "\n";
			//std::cout << "actionCombinationTook[i]: " << actionCombinationTook[i] << "\n";
			combinationValue[i][actionCombinationTook[i]] += result;
		}

		//std::cout << "childrenValue.size(): " << childrenValue.size() << "\n";
		//std::cout << "child_id: " << child_id << "\n";
		// Update child value
		//std::cout << "end updateValue\n";
		childrenValue[child_id] += result;
	}

	//Backpropagation in MCTS. Update number of visits, accummulated reward value and node reward bounds.
	void NaiveMCTSNode::backUp(NaiveMCTSNode* node, const double result, NaiveMCTSParameters& params)
	{
		NaiveMCTSNode* n = node;
		while (n->parentNode != nullptr){
			n->parentNode->updateValue(n->actionCombinationTook, result, params);
			n = n->parentNode;
		}
		//std::cout << "intermidium backup\n";
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

	//Selects the index of the child with the highest average reward value.
	int NaiveMCTSNode::bestAction(NaiveMCTSParameters& params, boost::mt19937& randomGenerator)
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
	double NaiveMCTSNode::normalize(const double aValue, const double aMin, const double aMax)
	{
		if (aMin < aMax)
			return (aValue - aMin) / (aMax - aMin);

		// if bounds are invalid, then return same value
		return aValue;
	}

	// helper function: adds a small random noise to a value and returns it.
	double NaiveMCTSNode::noise(const double input, const double epsilon, const double random)
	{
		return (input + epsilon) * (1.0 + epsilon * (random - 0.5));
	}

	// setter for the depth of this node and all nodes in the sub-tree this is root of.
	void NaiveMCTSNode::setDepth(int depth)
	{
		nodeDepth = depth;
		for (size_t i = 0; i < this->children.size(); i++) {
			children.at(i)->setDepth(depth + 1);
		}
	}

	// Increments the counter of nodes above this one.
	void NaiveMCTSNode::increaseTreeSize()
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

	void NaiveMCTSNode::printActionTookInfo() const {
		if (this->parentNode == nullptr){
			return;
		}

		auto actionCombination = this->actionCombinationTook;
		for (auto uid = 0; uid < actionCombination.size(); uid++) {
			auto action = this->parentNode->nodeActionSpace[uid][actionCombination[uid]];
			this->parentNode->gameState.printActionInfo(action);
			std::cout << " " << this->parentNode->entityActionSpaceSize[uid] << "\n";
		}
	}

	void NaiveMCTSNode::printNaiveInfo() const {
		/*each unit action*/
		std::cout << "Unit Action Info\n";
		for (int i = 0; i < nodeActionSpace.size(); i++) {
			for (int j = 0; j < nodeActionSpace[i].size(); j++) {
				auto action = this->nodeActionSpace[i][j];
				this->gameState.printActionInfo(action);
				std::cout << "Unit Action visit: " << combinationCount[i][j] <<
					", avg value: "<< combinationValue[i][j] / combinationCount[i][j] <<"\n";
			}
			std::cout << "\n";
		}

		std::cout << "\n";

		/*combination (children)*/
		std::cout << "Action Combination Info (Len: " << children.size() << ")\n";
		for (int i = 0; i < this->children.size(); i++) {
			auto actionCombination = this->children[i]->actionCombinationTook;
			for (auto uid = 0; uid < actionCombination.size(); uid++) {
				auto action = this->nodeActionSpace[uid][actionCombination[uid]];
				this->gameState.printActionInfo(action);
			}
			std::cout << "Child visit: " << childrenCount[i] << 
				", avg value: "<< childrenValue[i] / childrenCount[i] << "\n";
		}
		std::cout << "\n";
	}

	void NaiveMCTSNode::printNaiveInfo(std::string& prefix) const {
		if (isBottom)return;
		std::string new_prefix = prefix + "  ";
		/*each unit action*/
		std::cout << new_prefix<< "Unit Action Info\n";
		for (int i = 0; i < nodeActionSpace.size(); i++) {
			for (int j = 0; j < nodeActionSpace[i].size(); j++) {
				auto action = this->nodeActionSpace[i][j];
				std::cout << new_prefix;
				this->gameState.printActionInfo(action);
				std::cout << new_prefix<< "Unit Action visit: " << combinationCount[i][j] <<
					", avg value: " << combinationValue[i][j] / combinationCount[i][j] << "\n";
			}
		}

		std::cout <<"\n";

		/*combination (children)*/
		/*
		std::cout << new_prefix<< "Action Combination Info (Len: " << children.size() << ")\n";
		for (int i = 0; i < this->children.size(); i++) {
			auto actionCombination = this->children[i]->actionCombinationTook;
			for (auto uid = 0; uid < actionCombination.size(); uid++) {
				auto action = this->nodeActionSpace[uid][actionCombination[uid]];
				std::cout << new_prefix;
				this->gameState.printActionInfo(action);
			}
			std::cout << new_prefix<< "Child visit: " << childrenCount[i] << ", value: " << childrenValue[i] <<
				" avg value: " << childrenValue[i] / childrenCount[i] << "\n";
		}
		std::cout << "\n";
		*/
	}

	void NaiveMCTSNode::printNaiveTree(std::string& prefix) {
		this->printNaiveInfo(prefix);
		std::string new_prefix = prefix + "   ";
		for (int i = 0; i < this->children.size(); i++) {
			auto actionCombination = this->children[i]->actionCombinationTook;
			for (auto uid = 0; uid < actionCombination.size(); uid++) {
				auto action = this->nodeActionSpace[uid][actionCombination[uid]];
				std::cout << prefix;
				this->gameState.printActionInfo(action);
			}
			std::cout << prefix << "Child visit: " << this->childrenCount[i] <<
				", avg value: " << this->childrenValue[i] / this->childrenCount[i] << "\n";
			this->children[i]->printNaiveTree(new_prefix);
			std::cout << prefix << "---------------------------------------------\n";
		}
	}

	void NaiveMCTSNode::printActionInfo() const {
		if (this->parentNode != nullptr) {
			auto a = this->parentNode->actionSpace[this->childIndex];
			this->parentNode->gameState.printActionInfo(a);
		}
	}

	void NaiveMCTSNode::print() const
	{
		std::cout << "value: " << this->value / this->nVisits << "; nVisit: " << this->nVisits << "; children size: " << children.size()
			<< "; depth: " << nodeDepth << " ";
		printActionInfo();
	}
}