#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class BasicTBSResourceHeuristic : public StateHeuristic
	{
	private:
		bool initialized = false, ableToAttack=false, isResearchedMining=false, isResearchedDicipline=false,
			isBuiltBarracks=false;

		const double R_WORKER = 0.2, R_GOLDPROPORTION=0.8;
		double totalGold = 0.0;

		int self_city_id=-1, enemy_city_id=-1;
		Vector2f self_city, enemy_city;
		double city_max_health=-1.0;

	public:
		BasicTBSResourceHeuristic(int playerID, GameState initialState);
		void updateStateInfo(GameState& gameState, const int playerID);
		double evaluateGameState( const ForwardModel& forwardModel, GameState& gameState, const int playerID);

		double getProduction(GameState state);
		double getGold(GameState state);
		double getEntityHealth(GameState state, Vector2f position);

		std::string getName() const override { return "BasicTBSResourceHeuristic"; }
	};
}