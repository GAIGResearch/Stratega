#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class AbstractHeuristic : public StateHeuristic
	{
	private:
		std::map<std::string, int> attributeWeights;
		std::map<std::string, int> maxValue;

	public:
		AbstractHeuristic(std::map<std::string, int> attributeWeights, TBSGameState& gameState) : attributeWeights(attributeWeights)
		{
			for (const auto entry : attributeWeights) {
				const std::string parameterName = entry.first;
				maxValue[parameterName] = 0;
				for (const auto entityType : *gameState.entityTypes) {
					for (const auto parameter : entityType.second.parameters) {
						if (parameter.second.name == parameterName && parameter.second.maxValue > maxValue[parameterName]) {
							maxValue[parameterName] = parameter.second.maxValue;
						}
					}
				}
			}
		};

		AbstractHeuristic(TBSGameState& gameState)
		{
			for (const auto entityType : *gameState.entityTypes) {
				for (const auto parameter : entityType.second.parameters) {
					if (!attributeWeights.contains(parameter.second.name))
					{
						attributeWeights[parameter.second.name] = 1;
						maxValue[parameter.second.name] = parameter.second.maxValue;
					}
					else 
					{
						if (parameter.second.maxValue > maxValue[parameter.second.name])
						{
							maxValue[parameter.second.name] = parameter.second.maxValue;
						}
					}
				}
			}
		}

		double evaluateGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, const int playerID) override;
		static std::string getName() { return "AbstractHeuristic"; }
	};
}
