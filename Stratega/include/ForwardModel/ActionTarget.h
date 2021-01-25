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
			TechnologyReference
		};
		
		static ActionTarget createPositionActionTarget(Vector2f position);
		static ActionTarget createEntityActionTarget(int entityID);
		static ActionTarget createEntityTypeActionTarget(EntityTypeID entityTypeID);
		static ActionTarget createTechnologyEntityActionTarget(int technologyID);

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
		};

		Type targetType;
		Data data;

		// Private since this class should only be constructed using the static methods
		ActionTarget(const Type& type, const Data& data) : targetType(type), data(data) {};
	};
}
