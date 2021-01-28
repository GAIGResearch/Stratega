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
			
			if(action.actionTypeFlags == EndTickAction)
			{
				endTurn(state);
			}
			else if(action.actionTypeFlags==AbortContinuousAction)
			{

				if(action.targets[0].getType()==ActionTarget::PlayerReference)
				{
					auto& sourcePlayer = action.targets[0].getPlayer(state);
					int continuousActionID = action.targets[1].getContinuousActionID();

					//Search continuousAction to abort
					for (size_t i = 0; i < sourcePlayer.continuousAction.size(); i++)
					{
						if (sourcePlayer.continuousAction[i].continuousActionID == continuousActionID)
						{
							auto& actionType = state.getActionType(sourcePlayer.continuousAction[i].actionTypeID);

							//Execute OnAbort Effects				
							for (auto& effect : actionType.OnAbort)
							{
								effect->execute(state, *this, sourcePlayer.continuousAction[i].targets);
							}

							//Remove continuous action
							sourcePlayer.continuousAction.erase(sourcePlayer.continuousAction.begin() + i);
							i--;
						}
					}
				}
				else
				{
					auto& sourceEntity = action.targets[0].getEntity(state);
					int continuousActionID = action.targets[1].getContinuousActionID();

					//Search continuousAction to abort
					for (size_t i = 0; i < sourceEntity.continuousAction.size(); i++)
					{
						if (sourceEntity.continuousAction[i].continuousActionID == continuousActionID)
						{
							auto& actionType = state.getActionType(sourceEntity.continuousAction[i].actionTypeID);

							//Execute OnAbort Effects				
							for (auto& effect : actionType.OnAbort)
							{
								effect->execute(state, *this, sourceEntity.continuousAction[i].targets);
							}

							//Remove continuous action
							sourceEntity.continuousAction.erase(sourceEntity.continuousAction.begin() + i);
							i--;
						}
					}
				}

			}
			else if(action.actionTypeFlags==ContinuousAction)
			{
				auto& actionType = state.getActionType(action.actionTypeID);
				//If we are generating continuousAction we need to track them somehow
				//Using ID for each action for example				
				Action newAction = action;
				newAction.continuousActionID = state.continueActionNextID++;
				newAction.targets.emplace_back(ActionTarget::createContinuousActionActionTarget(newAction.continuousActionID));
								
				//If is continues we execute OnStart Effects
				//and we add the action to the list of continuous actions
				if (actionType.sourceType == ActionSourceType::Unit)
				{
					auto& type = state.actionTypes->at(actionType.id);
					for (auto& effect : type.OnStart)
					{
						effect->execute(state, *this, newAction.targets);
					}

					auto& executingEntity = newAction.targets[0].getEntity(state);
					executingEntity.continuousAction.emplace_back(newAction);

				}
				else if (actionType.sourceType == ActionSourceType::Player)
				{
					auto& type = state.actionTypes->at(actionType.id);
					for (auto& effect : type.OnStart)
					{
						effect->execute(state, *this, newAction.targets);
					}

					auto& executingPlayer = newAction.targets[0].getPlayer(state);
					executingPlayer.continuousAction.emplace_back(newAction);
				}

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
