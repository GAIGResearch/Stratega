#pragma once
#include <ForwardModel/EntityForwardModel.h>
#include <Representation/TBSGameState.h>
#include <Representation/Player.h>
#include <ForwardModel/EntityActionSpace.h>

namespace  SGA
{
	class TBSForwardModel : public EntityForwardModel
	{
	public:
		void advanceGameState(TBSGameState& state, const Action& action) const
		{
			if(action.isEndAction)
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

		void endTurn(TBSGameState& state) const
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

		virtual std::vector<Action> generateActions(TBSGameState& state) const
		{
			return (EntityActionSpace().generateActions(state, state.currentPlayer));
		}

		virtual std::vector<Action> generateActions(TBSGameState& state, int playerID) const
		{
			return (EntityActionSpace().generateActions(state, playerID));
		}

		virtual bool isValid(const TBSGameState& state, const Action& action) const { return true; }
		
		bool checkGameIsFinished(TBSGameState& state) const
		{
			if (state.currentTick >= state.tickLimit)
				return true;

			int numberPlayerCanPlay = 0;
			int winnerID = -1;
			for (Player& player : state.players)
			{
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
				state.winnerPlayerID=(winnerID);
				return true;
			}

			return false;
		}
		
		void moveEntity(TBSGameState& state, Entity& entity, Vector2f newPosition) const
		{
			entity.position = newPosition;
		}
	};
}
