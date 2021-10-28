#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

#include <Stratega/Utils/contains.h>

namespace SGA
{
	class AbstractHeuristic : public StateHeuristic
	{
	private:
		std::map<std::string, double> attributeWeights;
		std::map<std::string, double> attributeUValues;
		std::map<std::string, double> maxValue;
		std::map<std::string, double> minValue;

	public:
		AbstractHeuristic(std::map<std::string, double>& newAttributeWeights,
			std::map<std::string, double>& newAtributeUValues,
			GameState& gameState)
			: attributeWeights(std::move(newAttributeWeights)), attributeUValues(std::move(newAtributeUValues))
		{
			for (const auto& entry : newAttributeWeights) {
				const std::string parameterName = entry.first;
				maxValue[parameterName] = 0;
				for (const auto& entityType : gameState.getGameInfo()->getEntityTypes()) {
					for (const auto& parameter : entityType.second.getParameters()) {
						if (parameter.second.getName() == parameterName && parameter.second.getMaxValue() > maxValue[parameterName]) {
							maxValue[parameterName] = parameter.second.getMaxValue();
						}
						if (parameter.second.getName() == parameterName && parameter.second.getMinValue() < minValue[parameterName]) {
							minValue[parameterName] = parameter.second.getMinValue();
						}
					}
				}
			}
		};

		AbstractHeuristic(GameState& gameState)
		{
 			for (const auto& entityType : gameState.getGameInfo()->getEntityTypes()) {
				for (const auto& parameter : entityType.second.getParameters()) {
					if (attributeWeights.count(parameter.second.getName())==0 )
					{
						if (parameter.second.getName() == "Health")
						{
							attributeWeights[parameter.second.getName()] = 10;
							attributeUValues[parameter.second.getName()] = 5.0;
						}
						else
						{
							attributeWeights[parameter.second.getName()] = 1;
							attributeUValues[parameter.second.getName()] = 1;
						}
						maxValue[parameter.second.getName()] = parameter.second.getMaxValue();
						minValue[parameter.second.getName()] = parameter.second.getMinValue();
					}
					else
					{
						if (parameter.second.getMaxValue() > maxValue[parameter.second.getName()])
						{
							maxValue[parameter.second.getName()] = parameter.second.getMaxValue();
						}
						if (parameter.second.getMinValue() < minValue[parameter.second.getName()])
						{
							minValue[parameter.second.getName()] = parameter.second.getMinValue();
						}
					}
				}
			}
			attributeWeights["MovementPoints"] = 0;
		}

		double evaluateGameState(const ForwardModel& forwardModel, GameState& gameState, const int playerID) override;
		std::string getName() const override  { return "Abstract Heuristic"; }
	};
}
