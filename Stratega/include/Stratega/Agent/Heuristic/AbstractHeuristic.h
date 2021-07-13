#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class AbstractHeuristic : public StateHeuristic
	{
	private:
		std::map<std::string, double> attributeWeights;
		std::map<std::string, double> attributeUValues;
		std::map<std::string, int> maxValue;
		std::map<std::string, int> minValue;

	public:
		AbstractHeuristic(std::map<std::string, double>& attributeWeights,
			std::map<std::string, double>& attributeUValues,
			GameState& gameState)
			: attributeWeights(std::move(attributeWeights)), attributeUValues(std::move(attributeUValues))
		{
			for (const auto entry : attributeWeights) {
				const std::string parameterName = entry.first;
				maxValue[parameterName] = 0;
				for (const auto entityType : *gameState.gameInfo->entityTypes) {
					for (const auto parameter : entityType.second.parameters) {
						if (parameter.second.name == parameterName && parameter.second.maxValue > maxValue[parameterName]) {
							maxValue[parameterName] = parameter.second.maxValue;
						}
						if (parameter.second.name == parameterName && parameter.second.minValue < minValue[parameterName]) {
							minValue[parameterName] = parameter.second.minValue;
						}
					}
				}
			}
		};

		AbstractHeuristic(GameState& gameState)
		{
			for (const auto entityType : *gameState.gameInfo->entityTypes) {
				for (const auto parameter : entityType.second.parameters) {
					if (!attributeWeights.contains(parameter.second.name))
					{
						if (parameter.second.name == "Health")
						{
							attributeWeights[parameter.second.name] = 10;
							attributeUValues[parameter.second.name] = 5.0;
						}
						else
						{
							attributeWeights[parameter.second.name] = 1;
							attributeUValues[parameter.second.name] = 1;
						}
						maxValue[parameter.second.name] = parameter.second.maxValue;
						minValue[parameter.second.name] = parameter.second.minValue;
					}
					else
					{
						if (parameter.second.maxValue > maxValue[parameter.second.name])
						{
							maxValue[parameter.second.name] = parameter.second.maxValue;
						}
						if (parameter.second.minValue < minValue[parameter.second.name])
						{
							minValue[parameter.second.name] = parameter.second.minValue;
						}
					}
				}
			}
			attributeWeights["MovementPoints"] = 0;
		}

		double evaluateGameState(const ForwardModel& forwardModel, GameState& gameState, const int playerID) override;
		static std::string getName() { return "AbstractHeuristic"; }
	};
}
