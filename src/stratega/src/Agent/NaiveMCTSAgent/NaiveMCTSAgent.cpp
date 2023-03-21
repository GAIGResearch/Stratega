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
		state.printBoard();
		///*
		std::cout << "\nActionSpace: \n";

		auto actionSpace = forwardModel.generateActions(state, getPlayerID());
		for(auto action: actionSpace){
			state.printActionInfo(action);
		}
		//*/

        //Initialize the budget for this action call.
        parameters_.resetCounters(timer);
        //parameters_.printDetails();
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
				return ActionAssignment::fromSingleAction(action);
			}
			//std::cout << "loop no return\n";
		}

		/* if the action sequence is not executed completely */
		if(parameters_.actionSequence.size() == 0){
			//state.printBoard();

			// create a new node
			const auto processedForwardModel = parameters_.preprocessForwardModel(forwardModel);
			rootNode = std::make_unique<NaiveMCTSNode>(*processedForwardModel, state, std::vector<int>(), getPlayerID());
			//std::cout << "created a root node\n";
			// node searching
			rootNode->searchMCTS(*processedForwardModel, parameters_, getRNGEngine());
			
			//rootNode->printNaiveInfo();

			/*assign root node action to parameters action sequence*/
			std::vector<double> averageValue = {};
			for(int i = 0; i< rootNode->children.size(); i++){
				std::cout << "print children " << i << " actions:\n";
				for (int j = 0; j < rootNode->children[i]->actionCombinationTook.size(); j++) {
					int unitActionID = rootNode->children[i]->actionCombinationTook[j];
					auto a = rootNode->nodeActionSpace[j][unitActionID];
					state.printActionInfo(a);
				}
				//std::cout << "1\n";
				double single_avg_value = rootNode->childrenValue[i] / rootNode->childrenCount[i];
				averageValue.push_back(single_avg_value);
				std::cout << "value: " << rootNode->childrenValue[i] << ", count: "<< rootNode->childrenCount[i]  << ", avgValue: "<< single_avg_value << "\n";
			}
			//int which = std::distance(averageValue.begin(),
			//	std::max_element(averageValue.begin(), averageValue.end()));
			int which = std::distance(rootNode->childrenCount.begin(),
				std::max_element(rootNode->childrenCount.begin(), rootNode->childrenCount.end()));
			for (int i = 0 ; i < rootNode->children[which]->actionCombinationTook.size() ; i++){
				int unitActionID = rootNode->children[which]->actionCombinationTook[i];
				auto a = rootNode->nodeActionSpace[i][unitActionID];
				parameters_.actionSequence.push_back(a);
				if (a.getActionFlag() == ActionFlag::EndTickAction) {
					break;
				}
			}

			if(parameters_.actionSequence.size() == 0){
				std::cout << "[ERROR]: action to execute should not be zero after searching\n";
			}
			/*
			std::cout << "action sequences after searching:\n";
			for (auto a: parameters_.actionSequence){
				if (a.validate(state)) {
					state.printActionInfo(a);
				}
			}
			std::cout << "End action sequences.\n\n";
			//*/
		}

		auto action = parameters_.actionSequence[0];
		parameters_.actionSequence.erase(parameters_.actionSequence.begin());

		//std::cout << "-----------------------------------------------> returning Action ---------------------------->\n";
		bool isValid = action.validate(state);
		//std::cout << "isValid: " << isValid << "\n";
		///*
		std::cout << "Action sequence size: " << parameters_.actionSequence.size()<<" , " << "Action Took: \n";
		state.printActionInfo(action);
		std::cout << "\n";
		//*/
		return ActionAssignment::fromSingleAction(action);

    }

}