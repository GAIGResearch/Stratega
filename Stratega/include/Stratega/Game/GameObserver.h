#pragma once
#include <Stratega/Representation/GameState.h>

namespace SGA
{
	/// <summary>
	/// An interface for observing events in a game.
	/// </summary>
	class GameObserver
	{
	public:
		virtual ~GameObserver() = default;

		/// <summary>
		/// This method is called when a new game is started.
		/// This method is never called a second time before onGameFinished is called. 
		/// </summary>
		/// <param name="initialState">The state at the start of the game.</param>
		/// <param name="forwardModel">The forward model used in the game.</param>
		virtual void onGameStarted(const GameState& /*initialState*/, const ForwardModel& /*forwardModel*/) {};
		/// <summary>
		/// This method is called whenever the game is advanced by one timestep.
		/// </summary>
		/// <param name="state">The state after the game has been advanced.</param>
		/// <param name="forwardModel">The forward model used in the game.</param>
		virtual void onGameStateAdvanced(const GameState& /*state*/, const ForwardModel& /*forwardModel*/) {};
		/// <summary>
		/// This method is called when the game has ended.
		/// This method is never called until onGameStarted has been called.
		/// </summary>
		/// <param name="finalState">The final state of the game.</param>
		/// <param name="forwardModel">The forward model used in the game.</param>
		virtual void onGameFinished(const GameState& /*finalState*/, const ForwardModel& /*forwardModel*/) {};
	};
}