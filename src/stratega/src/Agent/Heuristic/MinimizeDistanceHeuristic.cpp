#include <map>
#include <set>
#include <cmath>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>

namespace SGA
{
    MinimizeDistanceHeuristic::MinimizeDistanceHeuristic(GameState& gamestate) {
        double width = gamestate.getBoardWidth();
        double height = gamestate.getBoardHeight();
        maximum_distance = sqrt(width*width+height*height);
    }

    MinimizeDistanceHeuristic::MinimizeDistanceHeuristic() {

    }

	double MinimizeDistanceHeuristic::evaluateGameState(const ForwardModel& /*forwardModel*/, GameState& gameState, int playerID)
	{
		double score = 0.0;

		if (gameState.isGameOver())
		{
			if (gameState.getWinnerID() == playerID)
				return 0.0;
			else
				return 1.0;
		}

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

		double sumOfAverageDistances = 0;
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
				sumOfAverageDistances = sumOfDistances / static_cast<double>(opponentEntites.size());
			}
		}

		if (playerEntities.size() > 0) {
			score += sumOfAverageDistances / static_cast<double>(playerEntities.size());
		}
        score /= maximum_distance;

		return score;
	}
}
