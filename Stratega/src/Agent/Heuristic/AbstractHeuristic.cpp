#include <map>
#include <set>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>

namespace SGA
{

	double AbstractHeuristic::evaluateGameState(const TBSForwardModel& forwardModel, GameState& gameState, int playerID)
	{
		double score = 0.0;

		if (gameState.isGameOver)
		{
			if (gameState.winnerPlayerID == playerID)
				score += 1000;	// this score needs to be maximized. so winning gives you 1000 points, but in a fog of war game, this will never trigger
			else
				score -= 1000;
		}

		for (auto entry : attributeWeights)
		{
			const std::string parameterName = entry.first;
			const int parameterWeight = entry.second;
			
			if (parameterWeight != 0) {

				std::vector<double> parameterValues;
				double sum = 0;

				for (auto entity : gameState.entities) {
					const auto& entityType = gameState.gameInfo->getEntityType(entity.typeID);
					for (const auto& parameter : entityType.parameters)
					{
						if (parameter.second.name == parameterName)
						{
							// (u * x) / ((u*(x-minValue[parameterName])-x+maxValue[parameterName])
							const double x = entity.parameters[parameter.second.index];
							const double u = attributeUValues[parameterName];
							double URQValue = (u * x) / (u * (x - minValue[parameterName]) - x + maxValue[parameterName]);
							//double URQValue = entity.parameters[parameter.second.index] / maxValue[parameterName];
							
							if (entity.ownerID == playerID) {
								const double val = URQValue * parameterWeight;
								parameterValues.push_back(val);
								sum += val;
							}
							else {
								const double val = URQValue * -parameterWeight;
								parameterValues.push_back(val);
								sum += val;
							}
							break;
						}
					}
				}
				score += sum;
			}
		}


		return score;
	}
}
