#include <cassert>
#include <Stratega/ForwardModel/TBSForwardModel.h>

namespace SGA
{
	void TBSForwardModel::advanceGameState(GameState& state, const ActionAssignment& actions) const
	{
		assert(actions.getAssignmentCount() == 1);
		for(const auto& action : actions.getEntityActions())
		{
			advanceGameState(state, action.second);
			return;
		}
		for (const auto& action : actions.getPlayerActions())
		{
			advanceGameState(state, action.second);
			return;
		}
	}
	
	void TBSForwardModel::advanceGameState(GameState& state, const Action& action) const
	{
		if (action.actionTypeFlags == ActionFlag::EndTickAction)
		{
			endTurn(state);
		}
		else
		{
			//Execute the action
			executeAction(state, action);
		}

		//Remove entities
		for (size_t i = 0; i < state.entities.size(); i++)
		{
			if (state.entities[i].shouldRemove)
			{
				state.entities.erase(state.entities.begin() + i);
				i--;
			}
		}

		//Check game is finished
		state.isGameOver = checkGameIsFinished(state);
	}

	std::vector<Action> TBSForwardModel::generateActions(const GameState& state) const
	{
		return generateActions(state, state.currentPlayer);
	}

	void TBSForwardModel::endTurn(GameState& state) const
	{
		// Find the next player who's still able to play
		for (auto i = 1; i <= state.players.size(); i++)
		{
			int nextPlayerID = (state.currentPlayer + i) % state.players.size();
			auto& targetPlayer = state.players[nextPlayerID];

			// All players did play, we consider this as a tick
			if (nextPlayerID == 0)
			{
				endTick(state);
			}

			if (targetPlayer.canPlay)
			{
				state.currentPlayer = nextPlayerID;
				break;
			}
		}
	}

	bool TBSForwardModel::isValid(const GameState& /*state*/, const Action& /*action*/) const
	{
		return true;
	}

	bool TBSForwardModel::checkGameIsFinished(GameState& state) const
	{
		if (state.currentTick >= state.tickLimit)
			return true;

		int numberPlayerCanPlay = 0;
		int winnerID = -1;
		for (Player& player : state.players)
		{
			//Check if player won
			if (player.canPlay && checkPlayerWon(state, player))
			{
				winnerID = player.id;

				state.winnerPlayerID = (winnerID);
				return true;
			}
			
			if (player.canPlay && canPlayerPlay(state, player))
			{
				winnerID = player.id;
				numberPlayerCanPlay++;
			}
			else
			{
				player.canPlay = false;
			}
		}

		if (numberPlayerCanPlay <= 1)
		{
			state.winnerPlayerID = (winnerID);
			return true;
		}

		return false;
	}
}
