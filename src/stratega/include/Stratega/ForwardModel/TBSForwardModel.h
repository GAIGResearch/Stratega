#pragma once
#include <Stratega/ForwardModel/ForwardModel.h>

namespace  SGA
{
	struct GameState;

	/// <summary>
	/// Is the default <see cref="SGA::ForwardModel"/> for TBS games, it contains specific methods.
	/// Each <see cref="SGA::TBSForwardModel::advanceGameState()"/> call execute the received action and
	/// updates the entities that should be removed before checking if the game is over.
	/// The <see cref="SGA::TBSForwardModel::advanceGameState()"/> accepts a list of actions or one single action.
	/// </summary>
	class TBSForwardModel : public ForwardModel
	{
	public:
		/// <summary>
		/// Executes an action in a given <see cref="SGA::GameState"/> before updating
		/// the entities of the gamestate that should be removed and checking if the game is over.
		/// </summary>
		void advanceGameState(GameState& state, const Action& action) const override;
		/// <summary>
		/// Executes a list of actions.
		/// </summary>
		void advanceGameState(GameState& state, const ActionAssignment& action) const override;

		using ForwardModel::generateActions;

		std::vector<Action> generateActions(const GameState& state) const;
		
		std::unique_ptr<ForwardModel> clone() const override;

		GameType getGameType() const override { return SGA::GameType::TBS; }

		/// <summary>
		/// End the turn of the current player and if all the player has played it ends the current game turn.
		/// </summary>
		void endTurn(GameState& state) const;


		/// <summary>
		/// Checks if the game is finished by current limit or because a player has won.
		/// </summary>
		bool checkGameIsFinished(GameState& state) const;
	};
}
