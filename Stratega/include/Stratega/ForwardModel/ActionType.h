#pragma once
#include <Stratega/ForwardModel/Effect.h>
#include <Stratega/ForwardModel/Condition.h>
#include <Stratega/ForwardModel/ActionSourceType.h>
#include <Stratega/ForwardModel/TargetType.h>

#include <string>
#include <vector>
#include <memory>

namespace SGA
{
	class Condition;

	/// <summary>
	/// Contains the definition an action. Which type is the source <see cref="SGA::ActionSourceType"/>, a set effects,
	/// preconditions and a list of action targets linked to their target conditions.
	/// If the action is continuous it will have a set of effects as events:
	/// OnStart, OnTick, OnComplete, OnAbort and the list of conditions that triggers the completed action.
	/// </summary>
	struct ActionType
	{
		std::string name;
		int id;
		ActionSourceType sourceType;

		int cooldownTicks;

		std::vector<std::pair<TargetType,std::vector<std::shared_ptr<Condition>>>> actionTargets;
		
		std::vector<std::shared_ptr<Condition>> preconditions;		
		std::vector<std::shared_ptr<Effect>> effects;

		//ContinuousAction
		bool isContinuous;
		//Condition that trigger the completion 
		std::vector<std::shared_ptr<Condition>> triggerComplete;
		//List of effects
		std::vector<std::shared_ptr<Effect>> OnStart;
		std::vector<std::shared_ptr<Effect>> OnTick;
		std::vector<std::shared_ptr<Effect>> OnComplete;
		std::vector<std::shared_ptr<Effect>> OnAbort;


		/// <summary>
		/// Returns a list of conditions linked to the searched target.
		/// </summary>
		const std::vector<std::shared_ptr<Condition>>& getTargetConditions(const TargetType& searchingTarget) const
		{
			for (auto& actionTarget : actionTargets)
			{
				if (actionTarget.first == searchingTarget)
					return actionTarget.second;
			}
		}
	};
}
