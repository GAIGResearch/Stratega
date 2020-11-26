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
			for(auto& sourceEntity : gameState.entities)
			{
				if (sourceEntity.owner != player)
					continue;				
				
				for(auto actionTypeID : sourceEntity.actionTypeIds)
				{
					if(!gameState.canExecuteAction(sourceEntity, actionTypeID))
					{
						continue;
					}
					
					//Find actionType
					auto& actionType = gameState.actionTypes->find(actionTypeID)->second;

					//Check preconditions
					for (const auto& precondition : actionType.preconditions)
					{
						if (!precondition->isFullfilled(gameState, {sourceEntity.id}))
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
							generateGroupTargets(gameState, actionType, targets);
							generateActions(targets, bucket, actionTypeID, sourceEntity);
							break;
						}							
						case TargetType::Position:
						{
							generatePositionTargets(gameState, actionType, targets, sourceEntity);
							generateActions(targets, bucket, actionTypeID, sourceEntity);
							
							break;
						}
						case TargetType::None:
						{
							generateSelfAction(gameState, sourceEntity, actionType, bucket);
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
			selfAction.actionTypeID = actionType.id;
			selfAction.targets.emplace_back(sourceEntity.id);
			bucket.emplace_back(selfAction);
		}

		virtual void generateGroupTargets(const GameState& gameState, const ActionType& actionType, std::vector<ActionTarget>& bucket)
		{
			for (auto& entityTarget : gameState.entities)
			{
				if (actionType.actionTargets.groupEntityTypes.find(entityTarget.typeID) != actionType.actionTargets.groupEntityTypes.end())
				{
					//Add precondition for group targets
					bucket.emplace_back(entityTarget.id);
				}
			}
		}

		virtual void generatePositionTargets(const GameState& gameState, const ActionType& actionType, std::vector<ActionTarget>& bucket, Entity& sourceEntity)
		{
			const Board& board = gameState.board;

			auto moveRange = actionType.actionTargets.shapeSize;
			auto startCheckPositionX = std::max<int>(0, sourceEntity.position.x - moveRange);
			auto endCheckPositionX = std::min<int>(board.getWidth() - 1, sourceEntity.position.x + moveRange);
			auto startCheckPositionY = std::max<int>(0, sourceEntity.position.y - moveRange);
			auto endCheckPositionY = std::min<int>(board.getHeight() - 1, sourceEntity.position.y + moveRange);
		
			switch (actionType.actionTargets.shapeType)
			{
			case ShapeType::Square:
								
				for (int x = startCheckPositionX; x <= endCheckPositionX; x++)
				{
					for (int y = startCheckPositionY; y <= endCheckPositionY; y++)
					{
						bucket.emplace_back(Vector2f(x,y));
					}
				}
				
				break;
			case ShapeType::Circle:
				for (int x = startCheckPositionX; x <= endCheckPositionX; x++)
				{
					for (int y = startCheckPositionY; y <= endCheckPositionY; y++)
					{
						if(Vector2f(x,y).manhattanDistance(sourceEntity.position)<actionType.actionTargets.shapeSize)
							bucket.emplace_back(Vector2f(x, y));
					}
				}
				break;
			}
		};


		virtual void generateActions(std::vector<ActionTarget>& targetbucket, std::vector<Action>& actionBucket, int actionTypeID, Entity& sourceEntity)
		{
			for(auto& target : targetbucket)
			{
				//Generate the action
				Action newAction;
				newAction.actionTypeID = actionTypeID;
				newAction.targets.emplace_back(sourceEntity.id);
				newAction.targets.emplace_back(target);
				
				actionBucket.emplace_back(std::move(newAction));
			}
		}
	};
}