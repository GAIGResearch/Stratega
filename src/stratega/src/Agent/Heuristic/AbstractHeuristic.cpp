#include <map>
#include <set>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>

namespace SGA
{

	double AbstractHeuristic::evaluateGameState(const ForwardModel& /*forwardModel*/, GameState& gameState, int playerID)
	{
		double score = 0.0;

		if (gameState.isGameOver())
		{
			if (gameState.getWinnerID() == playerID)
				score += 1000;	// this score needs to be maximized. so winning gives you 1000 points, but in a fog of war game, this will never trigger
			else
				score -= 1000;
		}

		for (auto entry : attributeWeights)
		{
			const std::string parameterName = entry.first;
			const int parameterWeight = static_cast<int>(entry.second);

			if (parameterWeight != 0) {

				std::vector<double> parameterValues;
				double sum = 0;

				for (auto entity : gameState.getEntities()) {
					const auto& entityType = gameState.getGameInfo()->getEntityType(entity.getEntityTypeID());
					for (const auto& parameter : entityType.getParameters())
					{
						if (parameter.second.getName() == parameterName)
						{
							// (u * x) / ((u*(x-minValue[parameterName])-x+maxValue[parameterName])
							const double x = entity.getRawParameterAt(parameter.second.getIndex());
							const double u = attributeUValues[parameterName];
							double den = (u * (x - minValue[parameterName]) - (x + maxValue[parameterName]));

							if(den > 0.0)
							{
								double URQValue = (u * x) / den;
								//double URQValue = entity.parameters[parameter.second.index] / maxValue[parameterName];
							
								if (entity.getOwnerID() == playerID) {
									const double val = URQValue * parameterWeight;
									parameterValues.push_back(val);
									sum += val;
								}
								else {
									const double val = URQValue * -parameterWeight;
									parameterValues.push_back(val);
									sum += val;
								}
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
