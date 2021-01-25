#include <ForwardModel/Condition.h>
#include <Representation/Entity.h>
#include <Representation/GameState.h>

namespace  SGA
{
	HasResource::HasResource(const std::vector<FunctionParameter>& parameters) :
		resourceReference(parameters.at(0)),
		lowerBound(parameters.at(1))
	{
	}

	bool HasResource::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto targetResource = resourceReference.getParameterValue(state, targets);
		double lowerBound = this->lowerBound.getConstant(state,targets);
		
		return targetResource >= lowerBound;
	}

	SamePlayer::SamePlayer(const std::vector<FunctionParameter>& parameters)
	{
		
	}

	bool SamePlayer::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto& sourceEntity =targets[0].getEntityConst(state);
		auto& targetEntity =targets[1].getEntityConst(state);

		return sourceEntity.ownerID == targetEntity.ownerID;
	}

	InRange::InRange(const std::vector<FunctionParameter>& parameters)
		: sourceEntity(parameters.at(0)),
		targetEntity(parameters.at(1)),
		distance(parameters.at(2))
	{
	}

	bool InRange::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& source = sourceEntity.getEntity(state, targets);
		const auto& target = targetEntity.getEntity(state, targets);
		auto dist = distance.getConstant(state, targets);

		return source.position.distance(target.position) <= dist;
	}

	IsWalkable::IsWalkable(const std::vector<FunctionParameter>& parameters)
		: targetPosition(parameters[0])
	{
	}
	
	bool IsWalkable::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto pos = targetPosition.getPosition(state, targets);
		return state.board.getTile(static_cast<int>(pos.x), static_cast<int>(pos.y)).isWalkable&& state.getEntityAt(pos) == nullptr;
	}

	IsPlayerEntity::IsPlayerEntity(const std::vector<FunctionParameter>& parameters)
		: targetParam(parameters[0])
	{
	}

	bool IsPlayerEntity::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& entity = targetParam.getEntity(state, targets);
		return !entity.isNeutral();
	}
	
	IsResearched::IsResearched(const std::vector<FunctionParameter>& parameters) :
		playerParam(parameters[0]),
		technologyTypeParam(parameters[1])
	{
	}

	bool IsResearched::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);
		const auto& targetTechnology = technologyTypeParam.getTechnology(state, targets);
		
		return state.technologyTreeCollection->isResearched(targetPlayer.id, targetTechnology.id);
	}

	CanResearch::CanResearch(const std::vector<FunctionParameter>& parameters) :
		playerParam(parameters[0]),
		technologyTypeParam(parameters[1])
	{
	}

	bool CanResearch::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);
		const auto& targetTechnology = technologyTypeParam.getTechnology(state, targets);

		return state.technologyTreeCollection->canResearch(targetPlayer.id, targetTechnology.id);
	}

	CanSpawnCondition::CanSpawnCondition(const std::vector<FunctionParameter>& parameters)
		: sourceEntityParam(parameters[0]), targetEntityTypeParam(parameters[1])
	{
	}

	bool CanSpawnCondition::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		const auto& sourceEntity = sourceEntityParam.getEntity(state, targets);
		const auto& targetEntityType = targetEntityTypeParam.getEntityType(state, targets);

		// Check if we fullfill the technology-requirements for the target entity
		if(targetEntityType.requiredTechnologyID != TechnologyTreeType::UNDEFINED_TECHNOLOGY_ID && 
			!state.technologyTreeCollection->isResearched(sourceEntity.ownerID, targetEntityType.requiredTechnologyID))
		{
			return false;
		}
		
		// Check if we are allowed to spawn this entity
		const auto& sourceEntityType = state.getEntityType(sourceEntity.typeID);
		if(sourceEntityType.spawnableEntityTypes.find(targetEntityType.id) == sourceEntityType.spawnableEntityTypes.end())
		{
			return false;
		}

		return true;
	}


}
