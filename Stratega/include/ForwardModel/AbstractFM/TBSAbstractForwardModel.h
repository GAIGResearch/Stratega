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


		bool canPlayerPlay(Player& player) const
		{
			//if (player.state.get().fogOfWarId != -1 && player.id != player.state.get().fogOfWarId)
			//	return true;

			//switch (winCondition)
			//{
			//case WinConditionType::UnitAlive:
			//{
			//	bool hasKing = false;
			//	std::vector<TBSUnit*> units = player.getUnits();

			//	for (auto& unit : units)
			//	{
			//		//Check if player has units
			//		if (unit->getUnitTypeID() == unitTypeID)
			//		{
			//			hasKing = true;
			//		}
			//	}

			//	if (!hasKing)
			//	{
			//		return false;
			//	}

			//	break;
			//}
			//case WinConditionType::LastManStanding:
			//{
			//	std::vector<TBSUnit*> units = player.getUnits();

			//	if (units.empty())
			//	{
			//		return false;
			//	}
			//	break;
			//}
			//}

			return true;
		}
		
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
		
	};
}
