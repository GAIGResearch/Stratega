#include <Stratega/ForwardModel/Condition.h>
#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/GameState.h>

namespace  SGA
{
	ResourceLowerEqual::ResourceLowerEqual(const std::string exp, const std::vector<FunctionParameter>& parameters) : 
		resourceReference(parameters.at(0)),
		lowerBoundParameter(parameters.at(1)), 
		Condition(exp)
	{
	}

	bool ResourceLowerEqual::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto targetResource = resourceReference.getParameterValue(state, targets);
		double lowerBound = lowerBoundParameter.getConstant(state,targets);
		
		return targetResource <= lowerBound;
	}

	ResourceGreaterEqual::ResourceGreaterEqual(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		resourceReference(parameters.at(0)),
		lowerBoundParameter(parameters.at(1)),
		Condition(exp)
	{
	}

	bool ResourceGreaterEqual::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto targetResource = resourceReference.getParameterValue(state, targets);
		double lowerBound = lowerBoundParameter.getConstant(state,targets);
		
		return targetResource >= lowerBound;
	}


	HasElapsedTime::HasElapsedTime(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		lowerBoundParameter(parameters.at(0)),
		Condition(exp)
	{
	}

	bool HasElapsedTime::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		double lowerBound = lowerBoundParameter.getConstant(state, targets);
		if(targets[0].getType()==ActionTarget::EntityReference)
		{
			auto& sourceEntity = targets[0].getEntityConst(state);

			for (const auto& action : sourceEntity.getContinuousActions())
			{
				if (action.continuousActionID == targets[2].getContinuousActionID())
				{
					//We reached the action
					//Tick amount
					if (action.elapsedTicks >= lowerBound)
						return true;
				}
			}
		}
		else if (targets[0].getType() == ActionTarget::PlayerReference)
		{
			auto& sourceEntity = targets[0].getPlayerConst(state);

			for (auto& action : sourceEntity.continuousAction)
			{
				if (action.continuousActionID == targets[2].getContinuousActionID())
				{
					//We reached the action
					//Tick amount
					if (action.elapsedTicks >= lowerBound)
						return true;
				}
			}
		}
		
		return false;
	}
	
	SamePlayer::SamePlayer(const std::string exp, const std::vector<FunctionParameter>& /*parameters*/) : Condition(exp)
	{
	}

	bool SamePlayer::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto& sourceEntity =targets[0].getEntityConst(state);
		auto& targetEntity =targets[1].getEntityConst(state);

		return sourceEntity.getOwnerID() == targetEntity.getOwnerID();
	}

	InRange::InRange(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: sourceEntity(parameters.at(0)),
		targetEntity(parameters.at(1)),
		distance(parameters.at(2)),
		Condition(exp)
	{
	}

	bool InRange::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& source = sourceEntity.getEntity(state, targets);
		const auto& target = targetEntity.getPosition(state, targets);
		auto dist = distance.getConstant(state, targets);

		return source.getPosition().distance(target) <= dist;
	}

	IsWalkable::IsWalkable(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: targetPosition(parameters[0]),
		Condition(exp)
	{
	}
	
	bool IsWalkable::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto pos = targetPosition.getPosition(state, targets);
		Tile t = state.getTileAt({ static_cast<int>(pos.x), static_cast<int>(pos.y) });
		return t.isWalkable && state.getEntityAt(pos) == nullptr;
	}

	IsTile::IsTile(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: targetPosition(parameters[0]),
		targetTile(parameters[1]),
		Condition(exp)
	{
	}
	
	bool IsTile::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto pos = targetPosition.getPosition(state, targets);
		const TileType& tileType = targetTile.getTileType(state, targets);
		//Check if target tile is same as the tile
		Tile t = state.getTileAt({ static_cast<int>(pos.x), static_cast<int>(pos.y) });
		return t.getTileTypeID()==tileType.id;
	}

	IsPlayerEntity::IsPlayerEntity(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: targetParam(parameters[0]),
		Condition(exp)
	{
	}

	bool IsPlayerEntity::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& entity = targetParam.getEntity(state, targets);
		return !entity.isNeutral();
	}
	
	IsResearched::IsResearched(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		playerParam(parameters[0]),
		technologyTypeParam(parameters[1]),
		Condition(exp)
	{
	}

	bool IsResearched::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);
		const auto& targetTechnology = technologyTypeParam.getTechnology(state, targets);
		
		return state.isResearched(targetPlayer.id, targetTechnology.id);
	}

	HasNoEntity::HasNoEntity(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		playerParam(parameters[0]),
		entityTypeParam(parameters[1]),
		Condition(exp)
	{
	}

	bool HasNoEntity::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);

		auto entities = state.getPlayerEntities(targetPlayer.id);
		
		bool hasEntity = false;
		for (auto& entity : entities)
		{
			if (entity->getEntityTypeID() == entityTypeParam.getEntityType(state, targets).id)
				hasEntity = true;
		}

		return !hasEntity;
	}
	
	HasEntity::HasEntity(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		playerParam(parameters[0]),
		entityTypeParam(parameters[1]),
		Condition(exp)
	{
	}

	bool HasEntity::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);

		auto entities = state.getPlayerEntities(targetPlayer.id);

		for (auto& entity : entities)
		{
			if (entity->getEntityTypeID() == entityTypeParam.getEntityType(state, targets).id)
				return true;
		}
	
		return false;
	}

	
	CanResearch::CanResearch(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		playerParam(parameters[0]),
		technologyTypeParam(parameters[1]),
		Condition(exp)
	{
	}

	bool CanResearch::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);
		const auto& targetTechnology = technologyTypeParam.getTechnology(state, targets);

		return state.canResearch(targetPlayer.id, targetTechnology.id);
	}

	CanSpawnCondition::CanSpawnCondition(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: sourceEntityParam(parameters[0]), targetEntityTypeParam(parameters[1]),
		Condition(exp)
	{
	}

	bool CanSpawnCondition::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		int playerID = sourceEntityParam.getPlayerID(state, targets);
		//const auto& sourceEntity = sourceEntityParam.getEntity(state, targets);
		const auto& targetEntityType = targetEntityTypeParam.getEntityType(state, targets);

		// Check if we fullfill the technology-requirements for the target entity
		if(targetEntityType.requiredTechnologyID != TechnologyTreeType::UNDEFINED_TECHNOLOGY_ID && 
			!state.isResearched(playerID, targetEntityType.requiredTechnologyID))
		{
			return false;
		}
		
		// Check if we are allowed to spawn this entity
		//const auto& sourceEntityType = state.getEntityType(sourceEntity.typeID);
		const auto& spawneableEntities = sourceEntityParam.getSpawneableEntities(state, targets);
		if(spawneableEntities.find(targetEntityType.id) == spawneableEntities.end())
		{
			return false;
		}

		return true;
	}

	CanAfford::CanAfford(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: sourceParam(parameters[0]), costParam(parameters[1]),
		Condition(exp)
	{
	}

	bool CanAfford::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{

		//Get cost of target, parameterlist to look up and the parameters of the source
		const auto& cost = costParam.getCost(state, targets);
		const auto& parameterLookUp = sourceParam.getParameterLookUp(state, targets);
		const auto& parameters = sourceParam.getParameterList(state, targets);

		//Check if the source can pay the all the cost of the target
		for (const auto& idCostPair : cost)
		{
			const auto& param = parameterLookUp.at(idCostPair.first);
			if (parameters[param.index] < idCostPair.second)
				return false;
		}

		return true;
	}


}
