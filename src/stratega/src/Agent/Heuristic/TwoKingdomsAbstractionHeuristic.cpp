#include <Stratega/Agent/Heuristic/TwoKingdomsAbstractionHeuristic.h>


namespace SGA {

	TwoKingdomsAbstractionHeuristic::TwoKingdomsAbstractionHeuristic(int playerID, GameState initialState, double wC_, double wT_, double wR_) {
        wC=wC_; wT=wT_; wR=wR_;
        //assert wT+wC+wS == 1.0;
		THeuristic = std::make_unique<BasicTBSTechnologyHeuristic>(playerID, initialState);
        RHeuristic = std::make_unique<BasicTBSResourceHeuristic>(playerID, initialState);
        CHeuristic = std::make_unique<BasicTBSCombatHeuristic>(playerID, initialState);
		return;
	}

    void TwoKingdomsAbstractionHeuristic::updateStateInfo(GameState& state, const int playerID) {
        return;
    }

    /*
    * 1) Check the status of technology, reward for each new technology
    * 2) Defend player's base.
    */
    double TwoKingdomsAbstractionHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state, const int playerID) {
        ///*
        return wT * THeuristic->evaluateGameState(forwardModel, state, playerID) +
                    wC*CHeuristic->evaluateGameState(forwardModel, state, playerID) + 
                    wR*RHeuristic->evaluateGameState(forwardModel, state, playerID, false);
        //*/

        //return wT*THeuristic->evaluateGameState(forwardModel, state, playerID) +
        //            wC*CHeuristic->evaluateGameState(forwardModel, state, playerID);
    }

    double TwoKingdomsAbstractionHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state,
        const int playerID, bool isDebug) {
        return wT * THeuristic->evaluateGameState(forwardModel, state, playerID) +
                    wC*CHeuristic->evaluateGameState(forwardModel, state, playerID) + 
                    wR*RHeuristic->evaluateGameState(forwardModel, state, playerID, false);

        //return wT*THeuristic->evaluateGameState(forwardModel, state, playerID) +
        //            wC*CHeuristic->evaluateGameState(forwardModel, state, playerID);

    }

    double TwoKingdomsAbstractionHeuristic::getTechnologyReward(const ForwardModel& forwardModel, GameState& state, const int playerID) {
        return THeuristic->evaluateGameState(forwardModel, state, playerID);
    }
    double TwoKingdomsAbstractionHeuristic::getResourceReward(const ForwardModel& forwardModel, GameState& state, const int playerID) {
        return RHeuristic->evaluateGameState(forwardModel, state, playerID, false);
    }
    double TwoKingdomsAbstractionHeuristic::getCombatReward(const ForwardModel& forwardModel, GameState& state, const int playerID) {
        return CHeuristic->evaluateGameState(forwardModel, state, playerID);
    }

    double TwoKingdomsAbstractionHeuristic::getProduction(GameState state){
        return state.getPlayerParameter(state.getCurrentTBSPlayer(), "Prod");
    }

    double TwoKingdomsAbstractionHeuristic::getGold(GameState state){
        return state.getPlayerParameter(state.getCurrentTBSPlayer(), "Gold");
    }

    double TwoKingdomsAbstractionHeuristic::getEntityHealth(GameState state, Vector2f position) {
        auto target_entity = state.getEntityAt(position);
        double paramsValue = target_entity->getParameter("Health");
        if (paramsValue >= 0.0) {
            return paramsValue;
        }
        //std::cout<<"[Error]: passed in invalid parameter name to search"<<std::endl;
    }

}