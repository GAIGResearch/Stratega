#include <cassert>
#include <Stratega/ForwardModel/TBSForwardModel.h>
#pragma warning(disable: 5045)
namespace SGA
{
	void TBSForwardModel::advanceGameState(GameState& state, const ActionAssignment& actions) const
	{
		assert(actions.getAssignmentCount() == 1);
		if (actions.getEntityActions().size() > 0)
		{
			//Check entity has player
			if (state.getEntity(actions.getEntityActions().begin()->second.getSourceID())->getOwnerID() != -1)
			{
				auto ownerID = actions.getEntityActions().begin()->second.getOwnerID();
				if (state.getActionQueues().hasActionInPlayerQueue(ownerID))
					if (state.getActionQueues().getActionFromPlayerQueue(ownerID).actionTypeID == actions.getEntityActions().begin()->second.getActionTypeID())
						state.getActionQueues().removeActionFromPlayerQueue(ownerID);
			}
				
			advanceGameState(state, actions.getEntityActions().begin()->second);
		}			
		else
		{
			if (state.getActionQueues().hasActionInPlayerQueue(actions.getPlayerActions().begin()->first))
				if (state.getActionQueues().getActionFromPlayerQueue(actions.getPlayerActions().begin()->first).actionTypeID == actions.getPlayerActions().begin()->second.getActionTypeID())
					state.getActionQueues().removeActionFromPlayerQueue(actions.getPlayerActions().begin()->first);
			advanceGameState(state, actions.getPlayerActions().begin()->second);
		}

		//Validate action queue
		state.getActionQueues().validateNextActions(state);

		//warning C4702: unreachable code
		/*for(const auto& action : actions.getEntityActions())
		{
			advanceGameState(state, action.second);
			return;
		}
		for (const auto& action : actions.getPlayerActions())
		{
			advanceGameState(state, action.second);
			return;
		}*/
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

		executeOnAdvanceEffects(state);

		//Remove flagged entities
		auto& entities = state.getEntities();
		auto it = entities.begin();
		while (it != entities.end())
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
			auto& targetPlayer = state.getPlayers()[static_cast<size_t>(nextPlayerID)];

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