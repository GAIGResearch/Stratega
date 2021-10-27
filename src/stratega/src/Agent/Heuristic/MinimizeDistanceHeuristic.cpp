#include <map>
#include <set>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>

namespace SGA
{
	double MinimizeDistanceHeuristic::evaluateGameState(const ForwardModel& /*forwardModel*/, GameState& gameState, int playerID)
	{
		double score = 0.0;

		if (gameState.isGameOver())
		{
			if (gameState.getWinnerID() == playerID)
				score -= 1000;	// since this score should be minimized we need to deduct points for winning
			else
				score += 1000;
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

		return score;
	}
}
