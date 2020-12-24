#pragma once
#include <ForwardModel/ForwardModel.h>
#include <Representation/TBSGameState.h>
#include <Representation/Player.h>
namespace  SGA
{
	class TBSAbstractForwardModel: public ForwardModel<TBSGameState2>
	{
	public:
		void advanceGameState(TBSGameState2& state, const Action& action) const override
		{
			if(action.isEndAction)
			{
				endTurn(state);
			}
			//Execute the action
			action.execute(state,*this);

			//Update removed entities
			for (size_t i = 0; i < state.entities.size(); i++)
			{
				if (state.entities[i].shouldRemove)
				{
					state.entities.erase(state.entities.begin() + i);
					i -= 1;
				}
			}

			auto& actionType = state.getActionType(action.actionTypeID);
			if (actionType.sourceType == ActionSourceType::Unit)
			{
				auto& entity = targetToEntity(state, action.targets[0]);
				entity.canExecuteAction = false;
			}
			//else
			//{
			//	std::cout << "test";
			//}
				

			//Check game is finished
			state.isGameOver = checkGameIsFinished(state);
		}

		void endTurn(TBSGameState2& state) const
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
		
		void initTurn(TBSGameState2& state) const
		{
			for (Entity& entity : state.entities)
			{
				entity.canExecuteAction = true;
			}
		}


		bool canPlayerPlay(Player& player) const;
		
		bool checkGameIsFinished(TBSGameState2& state) const override
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
		void moveEntity(TBSGameState2& state, Entity& entity, Vector2f newPosition) const
		{
			entity.position = newPosition;
		}

		
	};
}
