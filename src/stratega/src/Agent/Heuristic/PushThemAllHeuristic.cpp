#include <map>
#include <set>
#include <cmath>
#include <Stratega/Agent/Heuristic/PushThemAllHeuristic.h>

namespace SGA
{
    PushThemAllHeuristic::PushThemAllHeuristic(int playerID, GameState& gamestate) {
        double width = gamestate.getBoardWidth();
        double height = gamestate.getBoardHeight();
        //maximum_distance = sqrt(width*width+height*height);
        maximum_distance = width + height;

        //std::set<int> opponentEntites = std::set<int>();
        //std::set<int> playerEntities = std::set<int>();
        int tmp_opponent_unit_size = 0, tmp_self_unit_size = 0;
        for (const auto& entity : gamestate.getEntities())
        {
            if (entity.getOwnerID() == (-playerID + 1))
            {
                tmp_opponent_unit_size++;
            }
            else if (entity.getOwnerID() == playerID)
            {
                tmp_self_unit_size++;
            }
        }
        initial_opponent_unit_size = tmp_opponent_unit_size;
        initial_self_unit_size = tmp_self_unit_size;
        maximum_sum_distance = initial_self_unit_size*maximum_distance;
    }

    PushThemAllHeuristic::PushThemAllHeuristic() {

    }

	double PushThemAllHeuristic::evaluateGameState(const ForwardModel& /*forwardModel*/, GameState& gameState, int playerID)
	{
		double score = 0.0;

        std::map<int, Vector2f> positions;
		std::set<int> opponentEntites = std::set<int>();
		std::set<int> playerEntities = std::set<int>();

        for (const auto& entity : gameState.getEntities())
		{
			positions.emplace(entity.getID(), entity.getPosition());
			if (entity.getOwnerID() == (-playerID+1))
			{
				opponentEntites.insert(entity.getID());
			}
			else if (entity.getOwnerID() == playerID)
			{
				playerEntities.insert(entity.getID());
			}
		}
        //std::cout<< playerEntities.size()<< " oppo: "<< opponentEntites.size()<< "\n";

		if (gameState.isGameOver())
		{
			if (gameState.getWinnerID() == playerID)
				return 1.0;
			else
				return 0.0;
		}

		if (opponentEntites.size() == 0)return 1.0;
		if (playerEntities.size() == 0)return 0.01;

        double selfUnitScore = 0.0, opponentUnitScore = 0.0;
        double distanceScore = 0.0;

        selfUnitScore = static_cast<double>(playerEntities.size())/initial_self_unit_size; // in [0, 1]
        opponentUnitScore = (initial_opponent_unit_size - opponentEntites.size())/initial_opponent_unit_size; // >=0

        double sumOfAverageDistances = 10000.0;

        // the average distance does not make sense when self unit number decrease, because it wil encourage the farest self unit kill itself

        // to avoid unit kill it self, distance score for each unit is
        // its distance to closest opponent. died unit has a distance of maximum distance.
        double sum_max_distance = (initial_opponent_unit_size-static_cast<double>(playerEntities.size()))*maximum_distance;
        if (opponentEntites.size() > 0)
        {
            for (const auto& playerUnit : playerEntities)
            {
                double tmpDistance = 0;
                auto a = positions[playerUnit];
                double closest_enemy_distance = maximum_distance;
                for (int opponentUnit : opponentEntites)
                {
					auto b = positions[opponentUnit];
					tmpDistance = abs(a.x - b.x) + abs(a.y - b.y);
                    if (tmpDistance <closest_enemy_distance )
                        closest_enemy_distance = tmpDistance;
				}
				sum_max_distance += closest_enemy_distance;
			}
		}
        distanceScore = (maximum_sum_distance - sum_max_distance)/maximum_sum_distance;

        /*if (playerEntities.size() > 0) {
            distanceScore = sumOfAverageDistances / static_cast<double>(playerEntities.size());
        }*/
        //distanceScore = 1.0-sumOfAverageDistances;


        //std::cout<< distanceScore << " " <<selfUnitScore<<  " " <<opponentUnitScore <<  "\n";
        score = 0.2*distanceScore + 0.4*selfUnitScore + 0.4*opponentUnitScore;
        //std::cout<<score<<"\n";
		return score;
	}
}
