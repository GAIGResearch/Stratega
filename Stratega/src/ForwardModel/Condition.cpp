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
		technologyReference(parameters.at(0))
	{
	}

	bool IsResearched::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		//Get technologyType
		auto& targetResource = technologyReference.getTechnology(state, targets);

		//Check if player has the type researched
		auto& sourceEntity = targets[0].getEntityConst(state);
		int playerID = sourceEntity.ownerID;

		return state.technologyTreeCollection->isResearched(playerID, targetResource.id);
	}

	CanResearch::CanResearch(const std::vector<FunctionParameter>& parameters) :
		technologyReference(parameters.at(0))
	{
	}

	bool CanResearch::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		//Get technologyType
		auto& targetResource = technologyReference.getTechnology(state, targets);

		//Check if player has the type researched
		auto& sourceEntity = targets[0].getEntityConst(state);
		int playerID = sourceEntity.ownerID;

		return state.technologyTreeCollection->canResearch(playerID, targetResource.id);
	}
}
