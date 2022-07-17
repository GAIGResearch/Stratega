#include <Stratega/Agent/Heuristic/AimToKingHeuristic.h>
#include <map>
#include <set>
#include <string>

namespace SGA {

	AimToKingHeuristic::AimToKingHeuristic(GameState& gameState)
	{
	   unitNum = gameState.getEntities().size();
	}

	double AimToKingHeuristic::evaluateGameState( const ForwardModel& forwardModel, GameState& gameState, const int playerID)
    {
        double score = 0.0;

        if(gameState.isGameOver()) {
            if(gameState.getWinnerID() == playerID)
                score += 2.0;  // since this score should be minimized we need to deduct points for
                            // winning
            else
                score -= 2.0;
            return score;
        }

        int our_unit_number = 0;
        int king_x = -1, king_y = -1;
        double total_distance = 0.0, mean_distance = 0.0;

        std::map< int, Vector2f > positions;
        std::set< int > opponentEntites = std::set< int >();
        std::set< int > playerEntities = std::set< int >();

        double opponent_king_hp = 200.0;
        for(const auto& entity : gameState.getEntities()) {
            positions.emplace(
                entity.getID(), entity.getPosition());  // entity.id, entity.position);

            //if(entity.getOwnerID()!= gameState.getCurrentTBSPlayer())
			if(entity.getOwnerID()!= playerID)
            {
                opponentEntites.insert(entity.getID());

                // auto& entityType = gameState.gameInfo->getEntityType(entity.typeID);
                auto& entityType = gameState.getGameInfo()->getEntityType(
                entity.getEntityTypeID());
                if(entityType.getName() == "King") {
                king_x = entity.x();
                king_y = entity.y();
                int paramID = 0;
                
                opponent_king_hp = entity.getParameter("Health");
               
                }
            } else {
                playerEntities.insert(entity.getID());
            }
        }

        for(const auto& entity : playerEntities) {
            total_distance += abs(positions[entity].x - king_x)
                            + abs(positions[entity].y - king_y);
        }
        mean_distance = total_distance / playerEntities.size();

        double maximum_distance = 400.0;
        score = 1.0 - mean_distance / maximum_distance;  // set to the mean of distance of our units to opponent
                                            // units, divided by a constant 50 to make it slower
                                            // than the gameOver score

        score += 1.0 - opponent_king_hp / 400.0;

        double Total_N_entities = 5.0;
        // double Total_N_entities = unitNum;
        Total_N_entities *= 4;  // 4 for scaling
        score += playerEntities.size() / Total_N_entities;
        score -= opponentEntites.size() / Total_N_entities;

        return (score + 2.0) / 4.0;
    }

}