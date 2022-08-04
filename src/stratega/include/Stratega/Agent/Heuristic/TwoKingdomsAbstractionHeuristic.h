#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSCombatHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSTechnologyHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSResourceHeuristic.h>

namespace SGA
{
	class TwoKingdomsAbstractionHeuristic : public StateHeuristic
	{

	private:

        std::shared_ptr<BasicTBSCombatHeuristic> CHeuristic;
        std::shared_ptr<BasicTBSTechnologyHeuristic> THeuristic;
        std::shared_ptr<BasicTBSResourceHeuristic> RHeuristic;

	public:
        double wC=0.0, wT=0.0, wR=0.0;

		TwoKingdomsAbstractionHeuristic(int playerID, GameState initialState, double wC_=0.33, double wT_=0.33, double wR_=0.33);
		//void initialize(int playerID, GameState state);
		void updateStateInfo(GameState& gameState, const int playerID);
		double evaluateGameState( const ForwardModel& forwardModel, GameState& gameState, const int playerID);
		double evaluateGameState( const ForwardModel& forwardModel, GameState& gameState, const int playerID, bool isDebug);

        double getTechnologyReward(const ForwardModel& forwardModel, GameState& gameState, const int playerID);
        double getResourceReward(const ForwardModel& forwardModel, GameState& gameState, const int playerID);
        double getCombatReward(const ForwardModel& forwardModel, GameState& gameState, const int playerID);

		double getProduction(GameState state);
		double getGold(GameState state);

		double getEntityHealth(GameState state, Vector2f position);

		std::string getName() const override { return "TwoKingdomsAbstractionHeuristic"; }
	};
}