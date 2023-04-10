#include <Stratega/Agent/NaiveMCTSAgent/NaiveMCTSAgent.h>
#include <Stratega/Agent/Heuristic/AimToKingHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSHeuristic.h>
#include <Stratega/Agent/Heuristic/PushThemAllHeuristic.h>

namespace SGA
{

    void NaiveMCTSAgent::init(GameState initialState, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
    {
        parameters_.PLAYER_ID = getPlayerID();
        // The heuristic is set here because some heuristics have parameters depending on the game state. e.g. num_units
        /*
            if (parameters_.heuristic == nullptr)
                parameters_.heuristic = std::make_unique<AbstractHeuristic>(initialState);
        */
        //parameters_.heuristic = std::make_unique<AimToKingHeuristic>(initialState);
        //parameters_.heuristic = std::make_unique<PushThemAllHeuristic>(getPlayerID(), initialState);
        if (parameters_.HEURISTIC == "ktk") {
            parameters_.heuristic = std::make_unique< AimToKingHeuristic >(initialState);
        }
        else if (parameters_.HEURISTIC == "pta") {
            parameters_.heuristic = std::make_unique< PushThemAllHeuristic >(getPlayerID(), initialState);
        }
        if (parameters_.budgetType == Budget::UNDEFINED)
            parameters_.budgetType = Budget::TIME;
        parameters_.opponentModel = std::make_shared<RandomActionScript>();
        if(is_debug)
            parameters_.printDetails();
    }


    ActionAssignment NaiveMCTSAgent::computeAction(
       GameState state, const ForwardModel& forwardModel, Timer timer)
    {
		//state.printBoard();
		/*
		std::cout << "\nActionSpace: \n";

		auto actionSpace = forwardModel.generateActions(state, getPlayerID());
		for(auto action: actionSpace){
			state.printActionInfo(action);
		}
		//*/

        //Initialize the budget for this action call.
        parameters_.resetCounters(timer);

		if(parameters_.actionSequence.size() > 0){
			//std::cout << "entering loop\n";
			auto action = parameters_.actionSequence[0];
			parameters_.actionSequence.erase(parameters_.actionSequence.begin());
			while (!action.validate(state) && parameters_.actionSequence.size() > 0){
				action = parameters_.actionSequence[0];
				parameters_.actionSequence.erase(parameters_.actionSequence.begin());
			}
			//std::cout << "end loop\n";
			if(action.validate(state)){
				/*
				std::cout << "Action Took: \n";
				state.printActionInfo(action);
				std::cout << "\n";
				*/
				parameters_.actionSequence.erase(parameters_.actionSequence.begin(), parameters_.actionSequence.end());
				return ActionAssignment::fromSingleAction(action);
			}
			//std::cout << "loop no return\n";
		}

		const auto wholeActionSpace = forwardModel.generateActions(state, getPlayerID());
		if(wholeActionSpace.size() == 1){
			Action endAction = Action::createEndAction(getPlayerID());
			return ActionAssignment::fromSingleAction(endAction);
		}

		/* if the action sequence is not executed completely */
		if(parameters_.actionSequence.size() == 0){
			//state.printBoard();

			/*create a new node*/
			const auto processedForwardModel = parameters_.preprocessForwardModel(forwardModel);
			rootNode = std::make_unique<NaiveMCTSNode>(*processedForwardModel, state, std::vector<int>(), getPlayerID());

			//std::cout << "start search\n";
			rootNode->searchMCTS(*processedForwardModel, parameters_, getRNGEngine());

			/*add root node action to action sequence for executing*/
			std::string prefix = "";
			//rootNode->printNaiveTree(prefix);
			//rootNode->printNaiveInfo();

			/*
			std::vector<double> averageValue = {};
			for(int i = 0; i< rootNode->children.size(); i++){
				double single_avg_value = rootNode->childrenValue[i] / rootNode->childrenCount[i];
				averageValue.push_back(single_avg_value);
			}
			int which = std::distance(averageValue.begin(),
				std::max_element(averageValue.begin(), averageValue.end()));
			//int which = std::distance(rootNode->childrenCount.begin(),
			//	std::max_element(rootNode->childrenCount.begin(), rootNode->childrenCount.end()));
			std::cout << "selected child ID: " << which << "\n";
			bool hasEnd = false;
			for (int i = 0 ; i < rootNode->children[which]->actionCombinationTook.size() ; i++){
				int unitActionID = rootNode->children[which]->actionCombinationTook[i];
				auto a = rootNode->nodeActionSpace[i][unitActionID];
				
				if (a.getActionFlag() == ActionFlag::EndTickAction) {
					hasEnd = true;
				} else{
					parameters_.actionSequence.push_back(a);
				}
			}
			if(hasEnd){
				Action endAction = Action::createEndAction(getPlayerID());
				parameters_.actionSequence.push_back(endAction);
			}
			*/
			//std::cout << 2 << "\n";
			std::vector< std::vector<double> > averageValue = {};
			bool hasEnd = false;
			for (int i = 0; i < rootNode->combinationValue.size(); i++) {
				if(rootNode->combinationValue[i].size()==0){
					averageValue.push_back(std::vector<double>());
					continue;
				}
				averageValue.push_back(std::vector<double>());
				for (int j = 0; j< rootNode->combinationValue[i].size(); j++){
					averageValue[i].push_back(rootNode->combinationValue[i][j] / rootNode->combinationCount[i][j]);
					//averageValue[i].push_back(rootNode->combinationCount[i][j]);
				}
				int which = std::distance(averageValue[i].begin(),
					std::max_element(averageValue[i].begin(), averageValue[i].end()));
				auto a = rootNode->nodeActionSpace[i][which];
				if (a.getActionFlag() == ActionFlag::EndTickAction) {
					hasEnd = true;
				}
				else {
					parameters_.actionSequence.push_back(a);
				}
			}
			if (hasEnd) {
				Action endAction = Action::createEndAction(getPlayerID());
				parameters_.actionSequence.push_back(endAction);
			}

			if(parameters_.actionSequence.size() == 0){
				std::cout << "[ERROR]: action to execute should not be zero after searching\n";
			}
		}
		//std::cout << "3333\n";
		auto action = parameters_.actionSequence[0];
		parameters_.actionSequence.erase(parameters_.actionSequence.begin());

		//std::cout << "-----------------------------------------------> returning Action ---------------------------->\n";
		bool isValid = action.validate(state);
		//std::cout << "isValid: " << isValid << "\n";
		/*
		state.printBoard();
		//rootNode->printNaiveInfo();
		std::cout << "Action sequence size: " << parameters_.actionSequence.size() << " , " << "Action Took: \n";
		state.printActionInfo(action);
		//std::cout << "\n";
		//system("pause");
		//*/
		return ActionAssignment::fromSingleAction(action);

    }

}