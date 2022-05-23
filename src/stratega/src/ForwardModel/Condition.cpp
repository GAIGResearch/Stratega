#include <Stratega/ForwardModel/Condition.h>
#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/GameState.h>

namespace  SGA
{
	ResourceLowerEqual::ResourceLowerEqual(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp),
		resourceReference(parameters.at(0)),
		lowerBoundParameter(parameters.at(1))
		
	{
	}

	bool ResourceLowerEqual::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto targetResource = resourceReference.getParameterValue(state, targets);
		double lowerBound = lowerBoundParameter.getConstant(state,targets);
		
		return targetResource <= lowerBound;
	}

	ResourceGreaterEqual::ResourceGreaterEqual(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp),
		resourceReference(parameters.at(0)),
		lowerBoundParameter(parameters.at(1))
		
	{
	}

	bool ResourceGreaterEqual::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto targetResource = resourceReference.getParameterValue(state, targets);
		double lowerBound = lowerBoundParameter.getConstant(state,targets);
		
		return targetResource >= lowerBound;
	}


	HasElapsedTime::HasElapsedTime(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp),
		lowerBoundParameter(parameters.at(0))
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
				if (action.getContinuousActionID() == targets[2].getContinuousActionID())
				{
					//We reached the action
					//Tick amount
					if (action.getElapsedTicks() >= lowerBound)
						return true;
				}
			}
		}
		else if (targets[0].getType() == ActionTarget::PlayerReference)
		{
			auto& sourceEntity = targets[0].getPlayerConst(state);

			for (auto& action : sourceEntity.getContinuousActions())
			{
				if (action.getContinuousActionID() == targets[2].getContinuousActionID())
				{
					//We reached the action
					//Tick amount
					if (action.getElapsedTicks() >= lowerBound)
						return true;
				}
			}
		}
		
		return false;
	}
	
	IsNeutral::IsNeutral(const std::string exp, const std::vector<FunctionParameter>& /*parameters*/) : Condition(exp)
	{
	}

	bool IsNeutral::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{		
		auto& targetEntity =targets[0].getEntityConst(state);

		return targetEntity.isNeutral();
	}

	HasInventoryFull::HasInventoryFull(const std::string exp, const std::vector<FunctionParameter>& /*parameters*/) : Condition(exp)
	{
	}

	bool HasInventoryFull::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{		
		auto& targetEntity =targets[0].getEntityConst(state);

		return !targetEntity.isInventoryFull();
	}

	HasNotInventoryFull::HasNotInventoryFull(const std::string exp, const std::vector<FunctionParameter>& /*parameters*/) : Condition(exp)
	{
	}

	bool HasNotInventoryFull::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{		
		auto& targetEntity =targets[0].getEntityConst(state);

		return targetEntity.isInventoryFull();
	}
	
	IsNotNeutral::IsNotNeutral(const std::string exp, const std::vector<FunctionParameter>& /*parameters*/) : Condition(exp)
	{
	}

	bool IsNotNeutral::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{		
		auto& targetEntity =targets[0].getEntityConst(state);

		return !targetEntity.isNeutral();
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

	IsPlayerID::IsPlayerID(const std::string exp, const std::vector<FunctionParameter>& parameters) : 
		Condition(exp),
		playerID(parameters.at(1))
	{
	}

	bool IsPlayerID::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto targetPlayerID = playerID.getConstant(state, targets);

		return targets[0].getPlayerID(state) == targetPlayerID;
	}

	DifferentPlayer::DifferentPlayer(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp),
		targetEntity(parameters.at(1))
	{
	}

	bool DifferentPlayer::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto& sourceEntity = targets[0].getEntityConst(state);
		//auto& targetEntity = targets[1].getEntityConst(state);
		auto& target = targetEntity.getEntity(state, targets);

		return sourceEntity.getOwnerID() != target.getOwnerID();
	}

	InRange::InRange(const std::string exp, const std::vector<FunctionParameter>& parameters)
		:Condition(exp),
		sourceEntity(parameters.at(0)),
		targetEntity(parameters.at(1)),
		distance(parameters.at(2))
		
	{
	}

	bool InRange::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& source = sourceEntity.getPosition(state, targets);
		const auto& target = targetEntity.getPosition(state, targets);
		auto dist = distance.getConstant(state, targets);
		//std::cout << "	InRange: " << source.getPosition().distance(target) <<"<=" << dist<<std::endl;
		return source.distance(target) <= dist;
	}

	OutRange::OutRange(const std::string exp, const std::vector<FunctionParameter>& parameters)
		:Condition(exp),
		sourceEntity(parameters.at(0)),
		targetEntity(parameters.at(1)),
		distance(parameters.at(2))
		
	{
	}

	bool OutRange::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& source = sourceEntity.getEntity(state, targets);
		const auto& target = targetEntity.getPosition(state, targets);
		auto dist = distance.getConstant(state, targets);
		//std::cout << "	InRange: " << source.getPosition().distance(target) <<"<=" << dist<<std::endl;
		return source.getPosition().distance(target) >= dist;
	}

	IsWalkable::IsWalkable(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Condition(exp), 
		targetPosition(parameters[0])
	{
	}
	
	bool IsWalkable::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto pos = targetPosition.getPosition(state, targets);
		Tile t = state.getTileAtConst({ static_cast<int>(pos.x), static_cast<int>(pos.y) });
		return t.isWalkable()/* && state.getEntityAtConst(pos) == nullptr*/;
	}

	IsOccupied::IsOccupied(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Condition(exp), 
		targetPosition(parameters[0])
	{
	}
	
	bool IsOccupied::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto pos = targetPosition.getPosition(state, targets);
		
		return state.getEntitiesAtConst(pos).size()!=0;
	}
	IsOccupiedGrid::IsOccupiedGrid(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Condition(exp), 
		targetPosition(parameters[0]),
		gridLevel(parameters[1])
	{
	}
	
	bool IsOccupiedGrid::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto pos = targetPosition.getPosition(state, targets);
		if (gridLevel.getType() == FunctionParameter::Type::Constant)
		{
			auto grid = gridLevel.getConstant(state, targets);

			return state.getEntitiesAtConst(pos, static_cast<int>(grid)).size() != 0;
		}
		else if (gridLevel.getType() == FunctionParameter::Type::EntityTypeReference ||
			gridLevel.getType() == FunctionParameter::Type::ArgumentReference)
		{
			auto type = gridLevel.getEntityType(state, targets);

			return state.getEntitiesAtConst(pos, type.getID()).size() != 0;
		}
		else
			return false;
	}

	IsNotOccupied::IsNotOccupied(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Condition(exp), 
		targetPosition(parameters[0])
	{
	}
	
	bool IsNotOccupied::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto pos = targetPosition.getPosition(state, targets);
		
		return state.getEntitiesAtConst(pos).size()==0;
	}

	IsNotOccupiedGrid::IsNotOccupiedGrid(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Condition(exp), 
		targetPosition(parameters[0]),
		gridLevel(parameters[1])
	{
	}
	
	bool IsNotOccupiedGrid::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto pos = targetPosition.getPosition(state, targets);
		if (gridLevel.getType() == FunctionParameter::Type::Constant)
		{
			auto grid = gridLevel.getConstant(state, targets);

			return state.getEntitiesAtConst(pos, static_cast<int>(grid)).size() == 0;
		}
		else if (gridLevel.getType() == FunctionParameter::Type::EntityTypeReference ||
			gridLevel.getType() == FunctionParameter::Type::ArgumentReference)
		{
			auto type = gridLevel.getEntityType(state, targets);

			return state.getEntitiesAtConst(pos, type.getGrid()).size() == 0;
		}
		else return false;
	}

	IsTile::IsTile(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: 
		Condition(exp), 
		targetPosition(parameters[0]),
		targetTile(parameters[1])
	{
	}
	
	bool IsTile::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto pos = targetPosition.getPosition(state, targets);
		const TileType& tileType = targetTile.getTileType(state, targets);
		//Check if target tile is same as the tile
		Tile t = state.getTileAtConst({ static_cast<int>(pos.x), static_cast<int>(pos.y) });
		return t.getTileTypeID()==tileType.getID();
	}

	IsPlayerEntity::IsPlayerEntity(const std::string exp, const std::vector<FunctionParameter>& parameters) : 
		Condition(exp), 
		targetParam(parameters[0])
	{
	}

	bool IsPlayerEntity::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& entity = targetParam.getEntity(state, targets);
		return !entity.isNeutral();
	}

	IsTickMultipleOf::IsTickMultipleOf(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp), 
		multipleParam(parameters[0])
	{
	}

	bool IsTickMultipleOf::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		double multipleTick = multipleParam.getConstant(state, targets);
		auto currentTick = state.getCurrentTick();

		if((currentTick % static_cast<int>(multipleTick)) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
		
	}

	IsTick::IsTick(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp), 
		tickParam(parameters[0])
	{
	}

	bool IsTick::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		double tick = tickParam.getConstant(state, targets);
		auto currentTick = state.getCurrentTick();

		if(tick==currentTick)
		{
			return true;
		}
		else
		{
			return false;
		}		
	}

	IsNotTick::IsNotTick(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp), 
		tickParam(parameters[0])
	{
	}

	bool IsNotTick::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		double tick = tickParam.getConstant(state, targets);
		auto currentTick = state.getCurrentTick();

		if(tick!=currentTick)
		{
			return true;
		}
		else
		{
			return false;
		}		
	}
	
	IsResearched::IsResearched(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp),
		playerParam(parameters[0]),
		technologyTypeParam(parameters[1])
	{
	}

	bool IsResearched::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);
		const auto& targetTechnology = technologyTypeParam.getTechnology(state, targets);
		
		return state.isResearched(targetPlayer.getID(), targetTechnology.id);
	}

	HasNoEntity::HasNoEntity(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp),
		playerParam(parameters[0]),
		entityTypeParam(parameters[1])
	{
	}

	bool HasNoEntity::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);

		auto entities = state.getPlayerEntities(targetPlayer.getID());
		
		bool hasEntity = false;
		for (auto& entity : entities)
		{
			if (entity.getEntityTypeID() == entityTypeParam.getEntityType(state, targets).getID())
				hasEntity = true;
		}

		return !hasEntity;
	}

	HasNoEntities::HasNoEntities(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp),
		playerParam(parameters[0])
	{
	}

	bool HasNoEntities::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);

		auto entities = state.getPlayerEntities(targetPlayer.getID());

		return entities.size()<=0;
	}
	

	HasNoBuff::HasNoBuff(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp),
		entityParam(parameters[0]),
		buffTypeParam(parameters[1])
		
	{
	}

	bool HasNoBuff::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto& buffType = buffTypeParam.getBuffType(state, targets);

		if (entityParam.getType() == FunctionParameter::Type::EntityPlayerReference)
		{
			auto& entity = entityParam.getEntity(state, targets);
			auto* player = state.getPlayer(entity.getOwnerID());
			return !player->hasBuff(buffType.getID());
		}
		else
		{
			auto& entity = entityParam.getEntity(state, targets);
			return !entity.hasBuff(buffType.getID());
		}
	}

	HasBuff::HasBuff(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp),
		entityParam(parameters[0]),
		buffTypeParam(parameters[1])
	{
	}

	bool HasBuff::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto& buffType = buffTypeParam.getBuffType(state, targets);

		if (entityParam.getType() == FunctionParameter::Type::EntityPlayerReference)
		{
			auto& entity = entityParam.getEntity(state, targets);
			auto* player = state.getPlayer(entity.getOwnerID());
			return player->hasBuff(buffType.getID());
		}
		else
		{
			auto& entity = entityParam.getEntity(state, targets);
			return entity.hasBuff(buffType.getID());
		}
	}
	
	HasEntity::HasEntity(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp),
		playerParam(parameters[0]),
		entityTypeParam(parameters[1])
	{
	}

	bool HasEntity::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);

		auto entities = state.getPlayerEntities(targetPlayer.getID());

		for (auto& entity : entities)
		{
			if (entity.getEntityTypeID() == entityTypeParam.getEntityType(state, targets).getID())
				return true;
		}
	
		return false;
	}

	
	CanResearch::CanResearch(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Condition(exp),
		playerParam(parameters[0]),
		technologyTypeParam(parameters[1])
	{
	}

	bool CanResearch::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);
		const auto& targetTechnology = technologyTypeParam.getTechnology(state, targets);

		return state.canResearch(targetPlayer.getID(), targetTechnology.id);
	}

	CanSpawnCondition::CanSpawnCondition(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Condition(exp), 
		sourceEntityParam(parameters[0]),
		targetEntityTypeParam(parameters[1])
	{
	}

	bool CanSpawnCondition::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		int playerID = sourceEntityParam.getPlayerID(state, targets);
		//const auto& sourceEntity = sourceEntityParam.getEntity(state, targets);
		const auto& targetEntityType = targetEntityTypeParam.getEntityType(state, targets);

		// Check if we fullfill the technology-requirements for the target entity
		if(targetEntityType.getRequiredTechID() != TechnologyTreeType::UNDEFINED_TECHNOLOGY_ID &&
			!state.isResearched(playerID, targetEntityType.getRequiredTechID()))
		{
			return false;
		}
		
		// Check if we are allowed to spawn this entity
		//const auto& sourceEntityType = state.getEntityType(sourceEntity.typeID);
		const auto& spawneableEntities = sourceEntityParam.getSpawnableEntities(state, targets);
		if(spawneableEntities.find(targetEntityType.getID()) == spawneableEntities.end())
		{
			return false;
		}

		return true;
	}

	CanAfford::CanAfford(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Condition(exp), 
		sourceParam(parameters[0]), costParam(parameters[1])
	{
	}

	bool CanAfford::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{

		//Get cost of target, parameterlist to look up and the parameters of the source
		const auto cost = costParam.getCost(state, targets);
		const auto& parameterLookUp = sourceParam.getParameterLookUp(state, targets);
		const auto& parameters = sourceParam.getParameterList(state, targets);

		//Check if the source can pay the all the cost of the target
		for (const auto& idCostPair : cost)
		{
			const auto& param = parameterLookUp.at(idCostPair.first);
			auto it = parameters.find(param.getIndex());
			if (it != parameters.end())
			{
				if (it->second < idCostPair.second)
					return false;
			}
			else
			{
				throw std::runtime_error("Parameter not found");
			}
		}

		return true;
	}

	CanEquipObject::CanEquipObject(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Condition(exp),
		entityParam(parameters[0]), targetParam(parameters[1])
	{
	}

	bool CanEquipObject::isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		//Equip object
		auto& entity = entityParam.getEntity(state, targets);
		auto& target = targetParam.getObject(state, targets);

		auto& groupTypes = target.getEntityType().getCanEquipGroupEntityTypes();

		if (groupTypes.count(entity.getEntityType().getID()))
		{
			//Check if slots that use the object are not used
			if (entity.checkSlotsAreNotInUse(target))
				return true;
			else
				return false;
		}			
		else
			return false;		
	}


}
