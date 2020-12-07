#pragma once
#include <ForwardModel/AbstractFM/ForwardModel.h>
#include <Representation/AbstractGS/TBSGameState2.h>
#include <Representation/AbstractGS/Player.h>
namespace  SGA
{
	class TBSAbstractForwardModel: public ForwardModel<TBSGameState2>
	{
	public:
		void advanceGameState(TBSGameState2& state, const Action& action) const override
		{
			//Execute the action
			action.execute(state,*this);

			//Update removed entities
			/*for (size_t i = 0; i < state.entities.size(); i++)
			{
				if (state.entities[i].shouldRemove)
				{
					state.entities.erase(state.entities.begin() + i);
					i -= 1;
				}
			}*/

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
			
		}
		
	};
}
