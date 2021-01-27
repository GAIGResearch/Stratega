#pragma once
#include <ForwardModel/Effect.h>
#include <ForwardModel/Condition.h>
#include <ForwardModel/ActionSourceType.h>
#include <ForwardModel/TargetType.h>

#include <string>
#include <vector>
#include <memory>

namespace SGA
{
	class Condition;
	
	struct ActionType
	{
		std::string name;
		int id;
		ActionSourceType sourceType;

		int cooldownTicks;
		
		TargetType actionTargets;
		std::vector<std::shared_ptr<Condition>> preconditions;
		std::vector<std::shared_ptr<Condition>> targetConditions;
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
	};
}
