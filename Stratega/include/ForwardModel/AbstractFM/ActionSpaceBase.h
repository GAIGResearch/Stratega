#pragma once
#include <ForwardModel/IActionSpace.h>
#include <ForwardModel/AbstractFM/Action.h>
#include <Representation/AbstractGS/GameState.h> // ToDo remove this shiat

namespace SGA
{
	template<typename GameState=GameState>
	class ActionSpaceBase : public IActionSpace<GameState, Action>
	{
	public:


		std::vector<Action> generateActions(GameState& gameState) override { return {}; }

		std::vector<Action> generateActions(GameState& gameState, int player)
		{
			std::vector<Action> bucket;
			for(auto& entity : gameState.entities)
			{
				if (entity.owner != player)
					continue;				
				
				for(auto actionTypeID : entity.actionTypeIds)
				{
					if(!gameState.canExecuteAction(entity, actionTypeID))
					{
						continue;
					}
					
					//Find actionType
					auto& actionType = gameState.actionTypes->find(actionTypeID)->second;

					//Check preconditions
					for (const auto& precondition : actionType.preconditions)
					{
						if (!precondition->isFullfilled(gameState, {entity.id}))
						{
							continue;
						}
					}
					
					//Generate the targets for this action
					std::vector<ActionTarget> targets;
					switch (actionType.actionTargets.targetType)
					{
						case TargetType::Entity:
						{					
							generateGroupTargets(gameState, entity, actionType, targets);
							break;
						}							
						case TargetType::Position:
						{
							
							generatePositionTargets(gameState, entity, actionType, targets);

							generateActions(targets, bucket, actionTypeID);
							
							break;
						}
						case TargetType::None:
						{
							generateSelfAction(gameState, entity, actionType, targets);
							break;
						}						
							
						default:
							std::runtime_error("Type not recognised");
							break;
					}

					
				}
			}
			
			
			return bucket;
		}

		virtual void generateSelfAction(const GameState& gameState, const Entity& sourceEntity, const ActionType& actionType, std::vector<Action>& bucket)
		{
	
			Action selfAction;
			selfAction.actionTypeID = actionType;

			selfAction.targets.emplace_back(sourceEntity.id);
			
		}

		virtual void generateGroupTargets(const GameState& gameState, const Entity& sourceEntity, const ActionType& actionType, std::vector<ActionTarget>& bucket)
		{
			bucket.emplace_back(sourceEntity.id);
			for (auto& entityTarget : gameState.entities)
			{

				if (actionType.actionTargets.groupEntityTypes.find(sourceEntity.typeID) != actionType.actionTargets.groupEntityTypes.end())
				{					
					bucket.emplace_back(entityTarget.id);
				}
			}
		}

		virtual void generatePositionTargets(const GameState& gameState, const Entity& sourceEntity, const ActionType& actionType, std::vector<ActionTarget>& bucket) {};


		virtual void generateActions(std::vector<ActionTarget>& targetbucket, std::vector<Action>& actionBucket, int actionTypeID,Action action)
		{
			for (size_t i = 0; i < targetbucket.size; i++)
			{
				//Generate the action
				Action newAction;
				newAction.actionTypeID = actionTypeID;
				newAction.targets.emplace_back(target);

				actionBucket.emplace_back(std::move(newAction));
			}
		}
	};
}