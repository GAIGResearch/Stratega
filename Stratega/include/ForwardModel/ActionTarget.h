#pragma once
#include <variant>
#include <Representation/Vector2.h>

namespace SGA
{
	struct GameState;
	struct Entity;
	
	class ActionTarget
	{	
	public:
		enum Type
		{
			Position,
			EntityReference,
			TechnologyReference
		};
		
		static ActionTarget createPositionActionTarget(Vector2f position);
		static ActionTarget createEntityActionTarget(int entityID);
		static ActionTarget createTechnologyEntityActionTarget(int technologyID);


		//References		
		const Entity& getEntityConst(const GameState& state) const;
		Entity& getEntity(GameState& state) const;

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
			int technologyID;
		};

		Type targetType;
		Data data;

		// Private since this class should only be constructed using the static methods
		ActionTarget(const Type& type, const Data& data) : targetType(type), data(data) {};

	};

	
	
	//typedef std::variant<int, Vector2f> ActionTarget;
	
	//Vector2f targetToPosition(const GameState& state, const ActionTarget& target);
	//const Entity& targetToEntity(const GameState& state, const ActionTarget& target);
	// Entity& targetToEntity( GameState& state,  const ActionTarget& target);
}