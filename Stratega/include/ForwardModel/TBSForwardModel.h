#pragma once
#include <ForwardModel/EntityForwardModel.h>
#include <Representation/TBSGameState.h>
#include <Representation/Player.h>
#include <ForwardModel/ActionSpaceBase.h>

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
			
			//Execute the action
			action.execute(state,*this);

			//Remove entities
			for (size_t i = 0; i < state.entities.size(); i++)
			{
				if (state.entities[i].shouldRemove)
				{
					state.entities.erase(state.entities.begin() + i);
					i--;
				}
			}

			auto& actionType = state.getActionType(action.actionTypeID);
			if (actionType.sourceType == ActionSourceType::Unit)
			{
				auto& entity = targetToEntity(state, action.targets[0]);
				entity.canExecuteAction = false;
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

				if (nextPlayerID == 0)
				{
					state.currentGameTurn++;
				}

				if (targetPlayer.canPlay)
				{
					state.currentPlayer = nextPlayerID;
					break;
				}
			}

			initTurn(state);
		}
		
		void initTurn(TBSGameState& state) const
		{
			for (Entity& entity : state.entities)
			{
				entity.canExecuteAction = true;
			}
		}

		virtual std::vector<Action> generateActions(TBSGameState& state) const
		{
			return (ActionSpaceBase<TBSGameState>().generateActions(state));
		}

		virtual std::vector<Action> generateActions(TBSGameState& state, int playerID) const
		{
			return (ActionSpaceBase<TBSGameState>().generateActions(state, playerID));
		}

		virtual bool isValid(const TBSGameState& state, const Action& action) const { return true; }
		
		bool checkGameIsFinished(TBSGameState& state) const
		{
			if (state.currentGameTurn >= state.turnLimit)
				return true;

			int numberPlayerCanPlay = 0;
			int winnerID = -1;
			for (Player& player : state.players)
			{
				if (player.canPlay && canPlayerPlay(player))
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
