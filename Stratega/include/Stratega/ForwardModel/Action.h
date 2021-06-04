#pragma once
#include <vector>
#include <Stratega/ForwardModel/ActionTarget.h>
namespace SGA
{
	/// <summary>
	/// Contains the last tick this action was executed and the action type ID.
	/// Used by forward models to check if the <see cref="SGA::Entity"/> or player can execute the action again after the cooldown.
	/// </summary>
	struct ActionInfo
	{
		int actionTypeID;
		int lastExecutedTick;
	};
	
	class EntityForwardModel;

	/// <summary>
	/// Used to define how the <see cref="SGA::EntityForwardModel::executeAction()"/> will execute this action.
	/// </summary>
	enum class ActionFlag
	{
		None = 1 << 0,
		EndTickAction = 1 << 1,
		ContinuousAction = 1 << 2,
		AbortContinuousAction = 1 << 3
	};

	/// <summary>
	/// Contains the main information of an action as the action type id that is linked to an <see cref="SGA::ActionType"/> 
	/// which defines the conditions and effects that should be executed or validated.
	/// , the owner and a list of source/targets <see cref="SGA::ActionTarget"/> of the action.
	///
	/// The action has an actionFlag to check if is an end tick action, continuous action or a normal action.
	/// </summary>
	struct Action
	{
		Action(const ActionType* actionType):
			actionTypeFlags(ActionFlag::None),
			actionType(actionType),
			ownerID(0),
			continuousActionID(-1),
			elapsedTicks(0)
		{
		}
		
		ActionFlag actionTypeFlags;
		
		//int actionTypeID;
		// Contains all targets involved in an action
		// UnitAction: Index 0 contains the source and Index 1 the target of the action//opposite
		// PlayerAction": Index 0 contains the target of the action
		std::vector<ActionTarget> targets;
		int ownerID;

		int continuousActionID;
		int elapsedTicks;
	private:
		const ActionType* actionType;
		
	public:		
		/// <summary>
		/// Execute a list of effects which are defined in the <see cref="SGA::ActionType"/> linked to this action.
		/// </summary>
		void execute(GameState& state, const EntityForwardModel& fm) const;
		/// <summary>
		/// Execute a list of conditions which are defined in the <see cref="SGA::ActionType"/> linked to this action.
		/// This method checks if the last time of the action execution is higher than the cooldown,
		/// and if all the preconditions and target conditions are fullfilled.
		/// It also checks that all the actions targets are still valid.
		/// </summary>
		bool validate(GameState& state) const;

		[[nodiscard]] bool isEntityAction() const;
		[[nodiscard]] bool isPlayerAction() const;
		[[nodiscard]] int getSourceID() const;
		int getActionTypeID() const;
		const ActionType& getActionType() const
		{
			return *actionType;
		}

		/// <summary>
		/// Generates an Action used by the game to end the tick/turn.
		/// </summary>
		static Action createEndAction(int playerID)
		{
			Action a(nullptr);
			a.actionTypeFlags = ActionFlag::EndTickAction;
			a.ownerID = playerID;
			a.targets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
			return a;
		}

		/// <summary>
		/// Generates an Action which the owner is a entity, used by the game to abort a continuous action.
		/// </summary>
		static Action createAbortAction(int playerID,int entityID, int continuousActionID)
		{
			Action a(nullptr);			
			a.ownerID = playerID;			
			a.actionTypeFlags = ActionFlag::AbortContinuousAction;
			a.continuousActionID = continuousActionID;
			a.targets.emplace_back(ActionTarget::createEntityActionTarget(entityID));
			a.targets.emplace_back(ActionTarget::createContinuousActionActionTarget(continuousActionID));
			return a;
		}

		/// <summary>
		/// Generates an Action which the owner is a player, used by the game to abort a continuous action.
		/// </summary>
		static Action createAbortAction(int playerID, int continuousActionID)
		{
			Action a(nullptr);
			a.ownerID = playerID;
			a.actionTypeFlags = ActionFlag::AbortContinuousAction;
			a.continuousActionID = continuousActionID;
			a.targets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
			a.targets.emplace_back(ActionTarget::createContinuousActionActionTarget(continuousActionID));
			return a;
		}
	};
}