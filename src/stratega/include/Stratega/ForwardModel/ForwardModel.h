#pragma once
#include <boost/random.hpp>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/ActionSpace.h>
#include <Stratega/ForwardModel/ActionAssignment.h>

#include "Condition.h"
#include "Effect.h"

namespace SGA
{
	enum class WinConditionType
	{
		LastManStanding,
		UnitAlive
	};

	enum class SourceOnTickEffectType
	{
		Entity,
		Player,
		GameState
	};
	
	struct OnTickEffect
	{
		SourceOnTickEffectType type;
		
		std::vector<std::shared_ptr<Condition>> conditions;
		std::vector<std::shared_ptr<Effect>> effects;

		//Entity
		std::unordered_set<EntityTypeID> validTargets;
	};

	struct OnEntitySpawnEffect
	{
		std::unordered_set<EntityTypeID> validTargets;
		std::vector<std::shared_ptr<Condition>> conditions;
		std::vector<std::shared_ptr<Effect>> effects;
	};

	/// <summary>
	/// Is the driving component of Stratega, with a provided gamestate it can generate a new set of available
	/// actions thanks to the <see cref="SGA::ActionSpace"/>.
	/// The returned actions store the action type and a list of sources and targets the action will be applied to.
	/// Each <see cref="SGA::RTSForwardModel"/> and <see cref="SGA::TBSForwardModel"/> by default contains the definition of how to advance the game
	/// and other utility methods used in each specific game type.
	/// When a player provides the forward model an action and a gamestate, it first checks the type of action
	/// and after it updates the action info before executing the action.
	/// The forward models can be extended by implementing forward model abstractions, can be useful for reducing the games
	/// complexity and ease the training and decision-making process of AI agents. 
	/// </summary>
	class ForwardModel
	{

	public:

		//Constructor/destructor
		virtual ~ForwardModel() = default;
		ForwardModel();

		/// <summary>
		/// Advances the game state passed by parameter with a given action. 
		/// </summary>
		/// <param name="state">State to be advanced. The actual object is modified by this call.</param>
		/// <param name="action">Action to execute in the game state</param>
		virtual void advanceGameState(GameState& state, const Action& action) const = 0;

		/// <summary>
		/// Advances the game state passed by parameter with a given action
		/// </summary>
		/// <param name="state">Game state to advance. The actual object is modified by this call.</param>
		/// <param name="action">ActionAssignment to execute in the game state.</param>
		virtual void advanceGameState(GameState& state, const ActionAssignment& action) const = 0;
		
		/// <summary>
		/// Returns a list of available actions in the given gamestate by the received player
		/// </summary>
		/// <param name="state">Game state for which actions are generated.</param>
		/// <param name="playerID">ID of the players to generate available actions for.</param>
		/// <returns>The list of available actions for player with ID 'playerID' in game state 'state'.</returns>
		std::vector<Action> generateActions(const GameState& state, int playerID) const;

		std::vector<Action> generateUnitActions(const GameState& state, Entity e, int playerID, bool generateEnd = true) const;

		std::vector<Action> generatePlayerActions(const GameState& state, int playerID, bool generateEnd = true) const;


		/// <summary>
		/// Generates actions in the given gamestate by the received player and fills the action vector passed by parameter.
		/// </summary>
		/// <param name="state">State of the game to generate actions for.</param>
		/// <param name="playerID">ID of the player for which actions are generated.</param>
		/// <param name="actionBucket">List of actions to populate by this method.</param>
		virtual void generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const;

		/// <summary>
		/// Clones the current forward model.
		/// </summary>
		/// <returns>A copy of this forward model.</returns>
		virtual std::unique_ptr<ForwardModel> clone() const = 0;

		/// <summary>
		/// Indicates the game type this forward model is for. GameType is an enum defined in GameState.h
		/// </summary>
		/// <returns>The game type of this forward model.</returns>
		virtual GameType getGameType() const = 0;

		/// <summary>
		/// Checks if a player has lost the game due to the game's lose conditions.
		/// </summary>
		/// <param name="state">State of the game to check whether the player has lost or not.</param>
		/// <param name="playerID">ID of the player to check their lose condition</param>
		/// <returns>True of the player lost.</returns>
		bool checkPlayerLost(const GameState& state, int playerID) const;

		/// <summary>
		/// Returns if a player won the game by attending to the winning conditions defined in the rules.
		/// </summary>
		/// <param name="state">State of the game to check whether the player won or not</param>
		/// <param name="playerID">ID of the player to be checked.</param>
		/// <returns>True if the player won</returns>
		bool checkPlayerWon(const GameState& state, int playerID) const;

		/// <summary>
		/// Spawns an entity in the game with the default spawn entity method. A list of On Spawn effects are 
		/// executed just after spawning the entity
		/// </summary>
		/// <param name="state">State to spawn the entity in.</param>
		/// <param name="entityType">Type of entity to spawn.</param>
		/// <param name="playerID">ID of the player that will own the entity.</param>
		/// <param name="position">Position where the entity will be spawned.</param>
		void spawnEntity(GameState& state, const EntityType& entityType, int playerID, const Vector2f& position) const;

		/// <summary>
		/// Returns the action space of this forward model
		/// </summary>
		/// <returns>Action space of this forward model.</returns>
		std::shared_ptr<ActionSpace> getActionSpace() const { return actionSpace; }

		/// <summary>
		/// Adds a list of conditions for the game to be won for a player.
		/// </summary>
		/// <param name="conditions">List of conditions that, once fullfiled, cause a player to WIN.</param>
		void addWinConditions(std::vector<std::shared_ptr<Condition>>& conditions);

		/// <summary>
		/// Adds a list of conditions for the game to be lost for a player.
		/// </summary>
		/// <param name="conditions">List of conditions that, once fullfiled, cause a player to LOSE.</param>
		void addLoseConditions(std::vector<std::shared_ptr<Condition>>& conditions);

		/// <summary>
		/// Returns a list of sub-lists with all WIN conditions. Each sub-list contains a group of conditions
		/// that must be fulfilled for the game to be over for a player. The game will be WON by a player if all
		/// conditions in a sub-list are fullfiled, for at least one of the sub-lists returned. 
		/// </summary>
		/// <returns>A list of sub-lists with all WIN conditions</returns>
		const std::vector<std::vector<std::shared_ptr<Condition>>>& getWinConditions() const { return winConditions; }

		/// <summary>
		/// Returns a list of sub-lists with all LOSE conditions. Each sub-list contains a group of conditions
		/// that must be fulfilled for the game to be over for a player. The game will be LOST by a player if all
		/// conditions in a sub-list are fullfiled, for at least one of the sub-lists returned. 
		/// </summary>
		/// <returns>A list of sub-lists with all LOSE conditions</returns>
		const std::vector<std::vector<std::shared_ptr<Condition>>>& getLoseConditions() const { return loseConditions; }

		/// <summary>
		/// Adds an OnTickEffect to the forward mode, which will be executed every game tick.
		/// </summary>
		/// <param name="ote">Effect to add.</param>
		void addOnTickEffect(OnTickEffect& ote);

		/// <summary>
		/// Adds an OnAdvanceEffect to the forward mode, which will be executed every game tick.
		/// </summary>
		/// <param name="ote">Effect to add.</param>
		void addOnAdvanceEffect(OnTickEffect& ote);

		/// <summary>
		/// Adds an OnEntitySpawnEffect to the forward mode, which will be executed every time an entity is spawned.
		/// </summary>
		/// <param name="ote">Effect to add.</param>
		void addOnEntitySpawnEffect(OnEntitySpawnEffect& ose);

		/// <summary>
		/// Returns all effects that are exxecuted on every tick of the game.
		/// </summary>
		const std::vector<OnTickEffect>& getOnTickEffects() const { return onTickEffects; }

		/// <summary>
		/// Returns all effects that are exxecuted every time an entity is spawned in the game.
		/// </summary>
		const std::vector<OnEntitySpawnEffect>& getOnEntitySpawnEffects() const { return onEntitySpawnEffects; }

		/// <summary>
		/// Returns all effects that are exxecuted every time an entity is spawned in the game.
		/// </summary>
		void modifyEntityByParameterByName(Entity& entity, std::string& parameterName, double newValue) const
		{
			modifyEntityParameterByIndex(entity, entity.getEntityType().getParameterByName(parameterName).getIndex(), newValue);
		}
		
		/// <summary>
		///  Modify entity parameter by ID
		/// </summary>
		/// <param name="entity">Entity to search parameter from</param>
		/// <param name="parameterID">Index of the parameter</param>
		/// <param name="newValue">New value of the parameter</param>
		void modifyEntityParameterByID(Entity& entity, int parameterID, double newValue) const
		{
			modifyEntityParameterByIndex(entity, entity.getEntityType().getParameter(parameterID).getIndex(), newValue);
		}
		
		/// <summary>
		/// Modify entity parameter by index
		/// </summary>
		/// <param name="entity">Entity to search parameter from</param>
		/// <param name="parameterIndex">Index of the parameter</param>
		/// <param name="newValue">New value of the parameter</param>
		/// <param name="gameInfo">Game info object with the information of the current game</param>
		void modifyEntityParameterByIndex(Entity& entity, int parameterIndex, double newValue) const;
		
		/// <summary>
		/// Modify player parameter by name
		/// </summary>
		/// <param name="entity">Entity to search parameter from</param>
		/// <param name="parameterName">Name of the parameter</param>
		/// <param name="newValue">New value of the parameter</param>
		/// <param name="gameInfo">Game info object with the information of the current game</param>
		void modifyPlayerByParameterByName(Player& player, std::string& parameterName, double newValue, GameInfo& gameInfo) const
		{
			modifyPlayerParameterByIndex(player, gameInfo.getPlayerParameter(parameterName).getIndex(), newValue);
		}
		
		/// <summary>
		/// Modify player parameter by ID
		/// </summary>
		/// <param name="player">Player to search parameter from</param>
		/// <param name="parameterID">ID of the parameter</param>
		/// <param name="newValue">New value of the parameter</param>
		/// <param name="gameInfo">Game info object with the information of the current game</param>
		void modifyPlayerParameterByID(Player& player, int parameterID, double newValue, const GameInfo& gameInfo) const
		{
			modifyPlayerParameterByIndex(player, gameInfo.getPlayerParameter(parameterID).getIndex(), newValue);
		}
		
		/// <summary>
		/// Modify a player parameter by index
		/// </summary>
		/// <param name="player">Player to search parameter from</param>
		/// <param name="parameterIndex">Index of the parameter</param>
		/// <param name="newValue">New value of the parameter</param>
		void modifyPlayerParameterByIndex(Player& player, int parameterIndex, double newValue) const;

		/// <summary>
		/// Modify state parameter by name
		/// </summary>
		/// <param name="state">State to search parameter from</param>
		/// <param name="parameterName">Name of the parameter</param>
		/// <param name="newValue">New value of the parameter</param>
		/// <param name="gameInfo">Game info object with the information of the current game</param>
		void modifyStateByParameterByName(GameState& state, std::string& parameterName, double newValue, GameInfo& gameInfo) const
		{
			modifyStateParameterByIndex(state, gameInfo.getStateParameter(parameterName).getIndex(), newValue);
		}
		
		/// <summary>
		/// Modify state parameter by ID
		/// </summary>
		/// <param name="state">State to search parameter from</param>
		/// <param name="parameterID">ID of the parameter</param>
		/// <param name="newValue">New value of the parameter</param>
		/// <param name="gameInfo">Game info object with the information of the current game</param>
		void modifyStateParameterByID(GameState& state, int parameterID, double newValue, const GameInfo& gameInfo) const
		{
			modifyStateParameterByIndex(state, gameInfo.getStateParameter(parameterID).getIndex(), newValue);
		}
		
		/// <summary>
		/// Modify a state parameter by index
		/// </summary>
		/// <param name="state">State to search parameter from</param>
		/// <param name="parameterIndex">Index of the parameter</param>
		/// <param name="newValue">New value of the parameter</param>
		void modifyStateParameterByIndex(GameState& state, int parameterIndex, double newValue) const;

	protected:

		/// <summary>
		/// Effects applied on every tick of the game.
		/// </summary>
		std::vector<OnTickEffect> onTickEffects;

		/// <summary>
		/// Effects applied after every advanced game is executed.
		/// </summary>
		std::vector<OnTickEffect> onAdvanceEffects;

		/// <summary>
		/// Effects applied when a new entity is spawned in the game.
		/// </summary>
		std::vector<OnEntitySpawnEffect> onEntitySpawnEffects;


		/// <summary>
		/// Set of conditions required for a player to win the game.
		/// </summary>
		std::vector<std::vector<std::shared_ptr<Condition>>> winConditions;

		/// <summary>
		/// Set of conditions required for a player to lose the game.
		/// </summary>
		std::vector<std::vector<std::shared_ptr<Condition>>> loseConditions;


		/// <summary>
		/// Action space (generator) for this forward model.
		/// </summary>
		std::shared_ptr<ActionSpace> actionSpace;

		/// <summary>
		/// Executes the action in the given game state. This is used by subclasses of this forward model to
		/// actually execute an action in the game.
		/// </summary>
		/// <param name="state">State where the action is executed. This game state will be altered.</param>
		/// <param name="action">Action to execute.</param>
		void executeAction(GameState& state, const Action& action) const;

		/// <summary>
		/// Executes an action of action_type ABORT
		/// </summary>
		/// <param name="state">State where the action is to be executed.</param>
		/// <param name="action">Action to execute.</param>
		void executeAbortContinuousAction(GameState& state, const Action& action) const;

		/// <summary>
		/// Executes an action of action_type CONTINUOUS
		/// </summary>
		/// <param name="state">State where the action is to be executed.</param>
		/// <param name="action">Action to execute.
		void executeContinuousAction(GameState& state, const Action& action) const;

		/// <summary>
		/// Executes a regular action in the state provided.
		/// </summary>
		/// <param name="state">State where the action is to be executed.</param>
		/// <param name="action">Action to execute.
		void executeNormalAction(GameState& state, const Action& action) const;


		/// <summary>
		/// After each turn/ tick the triggers from the configuration are executed and
		/// the continuous action are executed after checking if they still are feasible.
		/// In terms of real-time strategy games, we consider a turn/tick to be the
		/// time-span until an agent returns a complex action for all its entities.
		/// </summary>
		/// <param name="state">State to finish its tick/turn.</param>
		void endTick(GameState& state) const;


		/// <summary>
		/// Executes the OnTrigger effects in the state provided
		/// </summary>
		/// <param name="state">State where the trigger events are to be executed.</param>
		void executeOnTriggerEffects(GameState& state) const;

		/// <summary>
		/// Executes the OnAdvance effects in the state provided
		/// </summary>
		/// <param name="state">State where the trigger events are to be executed.</param>
		void executeOnAdvanceEffects(GameState& state) const;

		/// <summary>
		/// Verifies if the continuous actions of the game entities should complete.
		/// </summary>
		/// <param name="state">State where these continuous actions are checked.</param>
		void checkEntitiesContinuousActionIsComplete(GameState& state) const;

		/// <summary>
		/// Verifies if the continuous actions of the players should complete.
		/// </summary>
		/// <param name="state">State where these continuous actions are checked.</param>
		void checkPlayerContinuousActionIsComplete(GameState& state) const;

		/// <summary>
		/// Verifies and removes the expired buffs
		/// </summary>
		/// <param name="state">State where these buffs are checked.</param>
		void removeExpiredBuffs(GameState& state) const;

		/// <summary>
		/// Executes actions of each entitty type
		/// </summary>
		/// <param name="state">State where these actions are executed.</param>
		void executeOnTickEntityActions(GameState& state) const;

		/// <summary>
		/// It generates a default action space unique pointer.
		/// </summary>
		/// <returns>Returns an empty action space.</returns>
		std::unique_ptr<ActionSpace> generateDefaultActionSpace() const
		{
			return std::make_unique<ActionSpace>();
		}

		/// <summary>
		/// Sets the action space of this objet to the oen passed by parameter.
		/// </summary>
		/// <param name="newActionSpace">Action space to set.</param>
		void setActionSpace(std::unique_ptr<ActionSpace> newActionSpace)
		{
			this->actionSpace = std::move(newActionSpace);
		}

	};

}
