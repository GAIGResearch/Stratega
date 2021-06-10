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
	/// Contains the definition of an action. Which type is the source <see cref="SGA::ActionSourceType"/> , a set effects,
	/// preconditions and a list of action targets linked to their target conditions.
	/// If the action is continuous it will have a set of effects as events:
	/// OnStart, OnTick, OnComplete, OnAbort and the list of conditions that triggers the completed action.
	/// </summary>
	struct ActionType
	{
		/// <summary>
		/// Name of this action type.
		/// </summary>
		std::string name;

		/// <summary>
		/// Unique ID of this action type
		/// </summary>
		int id;

		/// <summary>
		///  Source of this action (entity, player...)
		/// </summary>
		ActionSourceType sourceType;

		/// <summary>
		/// Number of ticks/turn that must happen between two uses of this action.
		/// </summary>
		int cooldownTicks;

		/// <summary>
		/// List of preconditions for this action to be run. These are tier 1 conditions: if they can't be fulfilled the action is not
		/// possible. If the conditions are true, the action may be possible based on situational circumstances. 
		/// (Example: having a resource to create an entity but no space in the board to spawn it).
		/// </summary>
		std::vector<std::shared_ptr<Condition>> preconditions;

		/// <summary>
		/// List of target types of this action with their conditions. Each target is a pair <target, list<conditions>>.
		/// These are tier 2 conditions (i.e. situational), to be checked during action generation.
		/// </summary>
		std::vector<std::pair<TargetType,std::vector<std::shared_ptr<Condition>>>> actionTargets;


		/// <summary>
		/// Indicates if action is continuous: if the execution of this action extends beyond the tick it was started.
		/// </summary>
		bool isContinuous;

		/// <summary>
		/// Continuous actions finish when certain conditions are met. This vector indicates those conditions.
		/// </summary>
		std::vector<std::shared_ptr<Condition>> triggerComplete;

		/// <summary>
		/// List of effects this action causes when executed. This list is populated only for non continuous actions.
		/// </summary>
		std::vector<std::shared_ptr<Effect>> effects;

		/// <summary>
		/// For continuous actions, list of effects that are applied when this action starts.
		/// </summary>
		std::vector<std::shared_ptr<Effect>> OnStart;

		/// <summary>
		/// For continuous actions, list of effects that are applied on every tick/turn. This effect is *not* applied on the tick this action starts,
		/// but it *will* also be applied in the tick it ends.
		/// </summary>
		std::vector<std::shared_ptr<Effect>> OnTick;

		/// <summary>
		/// For continuous actions, list of effects that are applied when this action ends.
		/// </summary>
		std::vector<std::shared_ptr<Effect>> OnComplete;

		/// <summary>
		/// For continuous actions, list of effects that are applied when this action is aborted. Aborted actions do not execute the OnTick effect in
		/// the game tick they have been aborted.
		/// </summary>
		std::vector<std::shared_ptr<Effect>> OnAbort;

		/// <summary>
		/// Returns a list of conditions linked to the searched target.
		/// </summary>
		/// <param name="searchingTarget">Target type to search for.</param>
		/// <returns>Conditions associated to a given target type.</returns>
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
