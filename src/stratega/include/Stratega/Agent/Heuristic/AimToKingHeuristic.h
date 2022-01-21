#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class AimToKingHeuristic : public StateHeuristic
	{
	public:
		double evaluateGameState(const TBSForwardModel& forwardModel, GameState& gameState, const int playerID) override {
			double score = 0.0;

			if (gameState.isGameOver)
			{
				if (gameState.winnerPlayerID == playerID)
					score += 2.0;	// since this score should be minimized we need to deduct points for winning
				else
					score -= 2.0;
				return score;
			}

			
			int our_unit_number = 0;
			int king_x = -1, king_y = -1;
			double total_distance = 0.0, mean_distance = 0.0;

			std::map<int, Vector2f> positions;
			std::set<int> opponentEntites = std::set<int>();
			std::set<int> playerEntities = std::set<int>();

			double opponent_king_hp = 200.0;
			for (const auto& entity : gameState.entities)
			{
				positions.emplace(entity.id, entity.position);
				if (entity.ownerID != gameState.currentPlayer)
				{
					opponentEntites.insert(entity.id);

					auto& entityType = gameState.gameInfo->getEntityType(entity.typeID);
					if (entityType.name == "King") {
						king_x = entity.position.x; king_y = entity.position.y;
						int paramID = 0;
						for (auto& parameter : entity.parameters)
						{
							if (entityType.parameters.find(paramID++)->second.name == "Health")
							{
								opponent_king_hp = parameter;
								//std::cout << "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB " <<parameter << std::endl;
							}
						}
						
					}
				}
				else
				{
					playerEntities.insert(entity.id);
				}
			}

			for (const auto& entity : playerEntities)
			{
				total_distance += abs(positions[entity].x - king_x) + abs(positions[entity].y - king_y);
			}
			mean_distance = total_distance / playerEntities.size();

			double maximum_distance = 400.0;
			score = 1.0 - mean_distance/ maximum_distance; // set to the mean of distance of our units to opponent units, divided by a constant 50 to make it slower than the gameOver score

			score += 1.0 - opponent_king_hp / 400.0;

			double Total_N_entities = 5.0;
			Total_N_entities *= 4; // 4 for scaling
			score += playerEntities.size() / Total_N_entities;
			score -= opponentEntites.size() / Total_N_entities;

			return (score+2.0)/4.0;
		}
		static std::string getName() { return "UnitNumberHeuristic"; }
	};
}
