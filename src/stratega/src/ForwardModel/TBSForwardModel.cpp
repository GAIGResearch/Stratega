#include <cassert>
#include <stratega/ForwardModel/TBSForwardModel.h>

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
		if (action.getActionFlag() == ActionFlag::EndTickAction)
		{
			endTurn(state);
		}
		else
		{
			//Execute the action
			executeAction(state, action);
		}

		//Remove flagged entities
		auto& entities = state.getEntities();
		auto it = entities.begin();
		while(it != entities.end())
		{
			if (it->flagged())  it = entities.erase(it);
			else 				it++;
		}

		//Check game is finished
		state.setGameOver(checkGameIsFinished(state));
	}

	std::vector<Action> TBSForwardModel::generateActions(const GameState& state) const
	{
		return generateActions(state, state.getCurrentTBSPlayer());
	}

	void TBSForwardModel::endTurn(GameState& state) const
	{
		// Find the next player who's still able to play
		for (auto i = 1; i <= state.getNumPlayers(); i++)
		{
			int nextPlayerID = (state.getCurrentTBSPlayer() + i) % state.getNumPlayers();
			auto& targetPlayer = state.getPlayers()[nextPlayerID];

			// All players did play, we consider this as a tick
			if (nextPlayerID == 0)
			{
				endTick(state);
			}

			if (targetPlayer.canPlay())
			{
				state.setCurrentTBSPlayer(nextPlayerID);
				break;
			}
		}
	}


	bool TBSForwardModel::checkGameIsFinished(GameState& state) const
	{
		if (state.getCurrentTick() >= state.getTickLimit())
			return true;

		int numberPlayerCanPlay = 0;
		int winnerID = -1;
		for (Player& player : state.getPlayers())
		{
			//Check if player won
			if (player.canPlay() && checkPlayerWon(state, player.getID()))
			{
				winnerID = player.getID();

				state.setWinnerID(winnerID);
				return true;
			}
			
			if (player.canPlay() && !checkPlayerLost(state, player.getID()))
			{
				winnerID = player.getID();
				numberPlayerCanPlay++;
			}
			else
			{
				player.setCanPlay(false);
			}
		}

		if (numberPlayerCanPlay <= 1)
		{
			state.setWinnerID(winnerID);
			return true;
		}

		return false;
	}

	std::unique_ptr<ForwardModel> TBSForwardModel::clone() const
	{
		return std::make_unique<TBSForwardModel>(*this);
	}
}
