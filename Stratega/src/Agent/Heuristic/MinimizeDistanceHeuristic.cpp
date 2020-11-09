#include <map>
#include <set>
#include <Agent/Heuristic/MinimizeDistanceHeuristic.h>

namespace SGA
{

	double MinimizeDistanceHeuristic::evaluateGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, int playerID)
	{
		double score = 0.0;

		if (gameState.isGameOver)
		{
			if (gameState.getWinnerID() == playerID)
				score -= 1000;	// since this score should be minimized we need to deduct points for winning
			else
				score += 1000;
		}

		std::vector<TBSUnit>& units = gameState.getUnits();
		std::map<int, Vector2i> positions = std::map<int, Vector2i>();
		std::set<int> opponentUnits = std::set<int>();
		std::set<int> playerUnits = std::set<int>();

		for (const TBSUnit& unit : units)
		{
			positions.insert(std::pair<int, Vector2i>(unit.getUnitID(), unit.getPosition()));
			if (unit.getPlayerID() != gameState.currentPlayer)
			{
				opponentUnits.insert(unit.getUnitID());
			}
			else
			{
				playerUnits.insert(unit.getUnitID());
			}
			
		}

		double sumOfAverageDistances = 0;
		for (int playerUnit : playerUnits)
		{
			double sumOfDistances = 0;
			Vector2i a = positions[playerUnit];
			for (int opponentUnit : opponentUnits)
			{
				Vector2i b = positions[opponentUnit];
				sumOfDistances += abs(a.x - b.x) + abs(a.y - b.y);
			}
			sumOfAverageDistances = sumOfDistances / opponentUnits.size();
		}
		score += sumOfAverageDistances / playerUnits.size();

		return score;
	}
}
