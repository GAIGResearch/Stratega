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
				for(auto actionType : entity.actionTypeIds)
				{
					if(!gameState.canExecuteAction(entity, actionType))
					{
						continue;
					}
				}
			}
			for(auto& idTypePair : *gameState.actionTypes)
			{
				for(auto& sourceEntity : gameState.entities)
				{
					if (sourceEntity.owner != player)
						continue;

					switch (idTypePair.second.targetType)
					{
						case TargetType::Self: generateSelfActions(gameState, sourceEntity, idTypePair.second, bucket); break;
						case TargetType::Group: generateGroupActions(gameState, sourceEntity, idTypePair.second, bucket); break;
						case TargetType::Position: generatePositionActions(gameState, sourceEntity, idTypePair.second, bucket); break;
						default:;
					}
				}
			}
			
			return bucket;
		}

		virtual void generateSelfActions(const GameState& gameState, const Entity& sourceEntity, const ActionType& actionType, std::vector<Action>& bucket)
		{
			Action selfAction;
			selfAction.actionTypeID = actionType.id;
			selfAction.targets[0] = sourceEntity.id;
			
			emplaceIfLegal(std::move(selfAction), gameState, actionType, bucket);
		}
		
		virtual void generateGroupActions(const GameState& gameState, const Entity& sourceEntity, const ActionType& actionType, std::vector<Action>& bucket)
		{
			for(auto& entity : gameState.entities)
			{
				if(actionType.groupEntityTypes.find(entity.typeID) != actionType.groupEntityTypes.end())
				{
					Action currAction;
					currAction.actionTypeID = actionType.id;
					currAction.targets[0] = sourceEntity.id;
					currAction.targets[1] = entity.id;
					emplaceIfLegal(std::move(currAction), gameState, actionType, bucket);
				}
			}
		}
		
		virtual void generatePositionActions(const GameState& gameState, const Entity& sourceEntity, const ActionType& actionType, std::vector<Action>& bucket) {};

	protected:
		void emplaceIfLegal(Action&& action, const GameState& gameState, const ActionType& actionType, std::vector<Action>& bucket)
		{
			for (const auto& precondition : actionType.preconditions)
			{
				if (!precondition->isFullfilled(gameState, action))
				{
					return;
				}
			}

			bucket.emplace_back(std::move(action));
		}
	};
}