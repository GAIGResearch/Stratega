#pragma once
#include <Stratega/Game/GameCommunicator.h>
#include <Stratega/Representation/TBSGameState.h>
#include <Stratega/Game/TBSGame.h>

#include <random>
#include <thread>

namespace SGA
{
	class Agent;

	/// <summary>
	/// A GameCommunicator for turn-based games that represents the connection between an agent and the TBSGameRunner.
	/// At the start of the game, it will execute the agents <see cref="SGA::Agent::runTBS()"/> method in a new thread.
	/// Afterwards, this class can be used by the agent to query information and send actions. 
	/// </summary>
	class TBSGameCommunicator : public GameCommunicator
	{
	public:
		/// <param name="playerID">The ID of the player that the agent will be able to control.</param>
		explicit TBSGameCommunicator(int playerID);

		void init() override;
		void close() override;

		/// <summary>
		/// Returns the random number generator that should be used by the agent.
		/// Using this generator ensures a deterministic behaviour because the generator is based on the seed used by the GameRunner.
		/// </summary>
		std::mt19937& getRNGEngine();
		/// <summary>
		/// Queries the game state from the GameRunner and applies Fog of War.
		/// </summary>
		TBSGameState getGameState() const;
		/// <summary>
		/// Checks if the game is over without querying a copy of the game state.
		/// This function should be preferred over querying the game state and manually checking <see cref="SGA::GameState::isGameOver"/>.
		/// </summary>
		bool isGameOver() const;
		/// <summary>
		/// Checks if it's the agent's turn.
		/// This function should be preferred over querying the game state and manually checking <see cref = "SGA::GameState::currentPlayer" / >.
		/// </summary>
		bool isMyTurn() const;

		/// <summary>
		/// Sends the given action to the game runner to be executed.
		/// The function will not return until the action was actually executed, this ensures that <see cref="getGameState()" /> returns the updated game state the next time it is called.
		/// </summary>
		/// <param name="action">The action that should be executed.</param>
		void executeAction(Action action) const;
		
		//TODO private
		void setGame(TBSGame& newGame);
		void setAgent(std::unique_ptr<Agent> agent);
		void setRNGEngine(std::mt19937 engine);

	private:
		TBSGame* game;
		std::unique_ptr<Agent> agent;
		std::mt19937 rngEngine;
		std::thread thread;
	};

}
