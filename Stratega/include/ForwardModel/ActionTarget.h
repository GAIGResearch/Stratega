#pragma once
#include <Representation/Vector2.h>

namespace SGA
{
	struct GameState;
	struct Entity;
	struct EntityType;
	typedef int EntityTypeID;
	
	class ActionTarget
	{	
	public:
		enum Type
		{
			Position,
			EntityReference,
			EntityTypeReference,
			TechnologyReference,
			ContinuousActionReference
		};
		
		static ActionTarget createPositionActionTarget(Vector2f position);
		static ActionTarget createEntityActionTarget(int entityID);
		static ActionTarget createEntityTypeActionTarget(EntityTypeID entityTypeID);
		static ActionTarget createTechnologyEntityActionTarget(int technologyID);
		static ActionTarget createContinuousActionActionTarget(int continuousActionID);

		//References		
		const Entity& getEntityConst(const GameState& state) const;
		Entity& getEntity(GameState& state) const;
		const EntityType& getEntityType(const GameState& state) const;

		//RAW Values
		Vector2f getPosition() const;
		int getTechnologyID()const
		{
			return data.technologyID;
		}
		int getEntityID() const
		{
			return data.entityID;
		}
		int getContinuousActionID() const
		{
			return data.continuousActionID;
		}
		Type getType() const
		{
			return targetType;
		}
		
	private:
		union Data
		{
			Vector2f position;
			int entityID;
			EntityTypeID entityTypeID;
			int technologyID;
			int continuousActionID;
		};

		Type targetType;
		Data data;

		// Private since this class should only be constructed using the static methods
		ActionTarget(const Type& type, const Data& data) : targetType(type), data(data) {};
	};
}
