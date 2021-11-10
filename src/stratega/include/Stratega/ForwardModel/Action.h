#pragma once
#include <vector>
#include <Stratega/ForwardModel/ActionTarget.h>
#include <Stratega/ForwardModel/ActionSourceType.h>
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
	
	class ForwardModel;

	/// <summary>
	/// Used to define how the <see cref="SGA::ForwardModel::executeAction()"/> will execute this action.
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
	/// The owner and a list of source/targets <see cref="SGA::ActionTarget"/> of the action.
	/// The action has an actionFlag to check if is an end tick action, continuous action or a normal action.
	/// </summary>
	struct Action
	{

	private:
		/// <summary>
		/// Type associated with this action.
		/// </summary>
		const ActionType* actionType;

		/// <summary>
		/// ID of the owner of this action.
		/// </summary>
		int ownerID;

		/// <summary>
		/// For continuous action, ID of this action.
		/// </summary>
		int continuousActionID;

		/// <summary>
		/// Ticks elapsed since this action started.
		/// </summary>
		int elapsedTicks;

		/// <summary>
		/// Used to define how the <see cref="SGA::ForwardModel::executeAction()"/> will execute this action.
		/// </summary>
		ActionFlag actionTypeFlags;

		/// <summary>
		/// Vector with all targets involved in this action. For a UnitAction, index 0 contains the source and index 1 the target of the action.
		/// For a PlayerAction, index 0 contains the target of the action. 
		/// </summary>
		std::vector<ActionTarget> targets;


	public:

		Action(const ActionType* newActionType):
			actionType(newActionType),
			ownerID(0),
			continuousActionID(-1),
			elapsedTicks(0),
			actionTypeFlags(ActionFlag::None)
		{
		}

		/// <summary>
		/// Execute the effects defined in the <see cref="SGA::ActionType"/> linked to this action.
		/// </summary>
		/// <param name="state">Game state in which this action is executed</param>
		/// <param name="fm">Forward model used to execute the aciton.</param>
		void execute(GameState& state, const ForwardModel& fm) const;

		/// <summary>
		/// Checks if this action can be executed.
		/// It verifies if the conditions defined in the <see cref="SGA::ActionType"/> linked to this action are passed.
		/// This method also checks if the last time of the action execution is higher than the cooldown,
		/// and if all the preconditions and target conditions are fullfilled.
		/// It also checks that all the actions targets are valid.
		/// </summary>
		/// <param name="state">Game state in which this action could be executed</param>
		/// <returns>True if this action can be run in the game state passed by parameter</returns>
		bool validate(GameState& state) const;

		/// <summary>
		/// Checks if this action is to be executed over an entity.
		/// </summary>
		/// <returns>True if this action has an entity as target</returns>
		bool isEntityAction() const;

		/// <summary>
		/// Checks if this action is to be executed over a player.
		/// </summary>
		/// <returns>True if this action has a player as target</returns>
		bool isPlayerAction() const;

		/// <summary>
		/// Gets the player or entity ID of the target of this action
		/// </summary>
		/// <returns>The player or entity ID of the target of this action</returns>
		int getSourceID() const;

		/// <summary>
		/// Returns the action type ID of this action
		/// </summary>
		/// <returns>Returns the action type ID of this action</returns>
		int getActionTypeID() const;
		
		/// <summary>
		/// Returns the name of this action
		/// </summary>
		/// <returns>Returns the name of this action</returns>
		std::string getActionName() const;

		/// <summary>
		/// Checks if this action is to be executed over an entity.
		/// </summary>
		/// <returns>True if this action has an entity as target</returns>
		const ActionType& getActionType() const { return *actionType; }

		/// <summary>
		/// Returns the action source type
		/// </summary>
		/// <returns>Action source type</returns>
		ActionSourceType getActionSourceType() const;

		 
		/// <summary>
		/// Generates an Action used by the game to end the tick/turn.
		/// </summary>
		/// <param name="playerID">ID of the player for which the End action is created</param>
		/// <returns>Action that ends the current turn for the plater ID supplied</returns>
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
		/// <param name="playerID">Player ID for which this action is created.</param>
		/// <param name="entityID">Entity that owns this action to abort a continuous action.</param>
		/// <param name="continuousActionID">ID of the continuous action to abort</param>
		/// <returns></returns>
		static Action createAbortEntityAction(int playerID, int entityID, int continuousActionID)
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
		/// <param name="playerID">Player ID for which this action is created.</param>
		/// <param name="continuousActionID">ID of the continuous action to abort</param>
		/// <returns></returns>
		static Action createAbortPlayerAction(int playerID, int continuousActionID)
		{
			Action a(nullptr);
			a.ownerID = playerID;
			a.actionTypeFlags = ActionFlag::AbortContinuousAction;
			a.continuousActionID = continuousActionID;
			a.targets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
			a.targets.emplace_back(ActionTarget::createContinuousActionActionTarget(continuousActionID));
			return a;
		}

		/* Getters and setters for Action */


		int getOwnerID() const { return ownerID; }
		void setOwnerID(int id) { ownerID = id; }

		int getContinuousActionID() const { return continuousActionID; }
		void setContinuousActionID(int id) { continuousActionID = id; }

		int getElapsedTicks() const { return elapsedTicks; }
		void setElapsedTicks(int elapsed) { elapsedTicks = elapsed; }
		void incTicks() { elapsedTicks++; }

		const ActionFlag& getActionFlag() const { return actionTypeFlags; }
		void setActionFlag(ActionFlag flag) { actionTypeFlags = flag; }

		const std::vector<ActionTarget>& getTargets() const { return targets; }
		std::vector<ActionTarget>& getTargets() { return targets; }
		void setActionTargets(std::vector<ActionTarget>& actionTargets) { targets = actionTargets; }


	};
}