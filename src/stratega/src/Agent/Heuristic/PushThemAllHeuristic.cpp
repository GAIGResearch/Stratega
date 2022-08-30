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
            if (entity.getOwnerID() != playerID)
            {
                tmp_opponent_unit_size++;
            }
            else
            {
                tmp_self_unit_size++;
            }
        }
        initial_opponent_unit_size = tmp_opponent_unit_size;
        initial_self_unit_size = tmp_self_unit_size;
    }

    PushThemAllHeuristic::PushThemAllHeuristic() {

    }

	double PushThemAllHeuristic::evaluateGameState(const ForwardModel& /*forwardModel*/, GameState& gameState, int playerID)
	{
		double score = 0.0;

		if (gameState.isGameOver())
		{
			if (gameState.getWinnerID() == playerID)
				return 1.0;
			else
				return 0.0;
		}

        double selfUnitScore = 0.0, opponentUnitScore = 0.0;
        double distanceScore = 0.0;

		std::map<int, Vector2f> positions;
		std::set<int> opponentEntites = std::set<int>();
		std::set<int> playerEntities = std::set<int>();

		for (const auto& entity : gameState.getEntities())
		{
			positions.emplace(entity.getID(), entity.getPosition());
			if (entity.getOwnerID() != playerID)
			{
				opponentEntites.insert(entity.getID());
			}
			else
			{
				playerEntities.insert(entity.getID());
			}
		}

        selfUnitScore = static_cast<double>(playerEntities.size())/initial_self_unit_size; // in [0, 1]
        opponentUnitScore = (initial_opponent_unit_size - opponentEntites.size())/initial_opponent_unit_size; // >=0

        double sumOfAverageDistances = 10000.0;
        
        // the average distance does not make sense when self unit number decrase, because it wil encourage the farest self unit die
        if(true){
		    if (opponentEntites.size() > 0)
		    {
			    for (const auto& playerUnit : playerEntities)
			    {
				    double sumOfDistances = 0;
				    auto a = positions[playerUnit];
				    for (int opponentUnit : opponentEntites)
				    {
					    auto b = positions[opponentUnit];
					    sumOfDistances += abs(a.x - b.x) + abs(a.y - b.y);
				    }
				    double tmp_sumOfAverageDistances = sumOfDistances / static_cast<double>(opponentEntites.size()) / maximum_distance;
                    if (tmp_sumOfAverageDistances < sumOfAverageDistances) {
                        sumOfAverageDistances = tmp_sumOfAverageDistances;
                    }
			    }
		    }

            /*if (playerEntities.size() > 0) {
                distanceScore = sumOfAverageDistances / static_cast<double>(playerEntities.size());
            }*/
            distanceScore = 1.0-sumOfAverageDistances;
        }

        //std::cout<< distanceScore << " " <<selfUnitScore<<  " " <<opponentUnitScore <<  "\n";
        score = 0.2*distanceScore + 0.5*selfUnitScore + 0.3*opponentUnitScore;
		return score;
	}
}
