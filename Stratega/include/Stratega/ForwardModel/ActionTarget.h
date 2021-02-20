#pragma once

#include <string>
#include <unordered_set>
#include <variant>
#include <Stratega/Representation/Vector2.h>


namespace SGA
{
	struct GameState;
	struct Entity;

	class Player;
	struct EntityType;
	typedef int EntityTypeID;
	
	class ActionTarget
	{	
	public:
		enum Type
		{
			Position,
			EntityReference,
			PlayerReference,
			EntityTypeReference,
			TechnologyReference,
			ContinuousActionReference
		};
		
		static ActionTarget createPositionActionTarget(Vector2f position);
		static ActionTarget createEntityActionTarget(int entityID);
		static ActionTarget createPlayerActionTarget(int playerID);
		static ActionTarget createTechnologyEntityActionTarget(int technologyID);
		static ActionTarget createContinuousActionActionTarget(int continuousActionID);
		static ActionTarget createEntityTypeActionTarget(EntityTypeID entityTypeID);


		//References		
		const Entity& getEntityConst(const GameState& state) const;
		Entity& getEntity(GameState& state) const;
		Player& getPlayer(GameState& state) const;
		const Player& getPlayerConst(const GameState& state) const;
		const EntityType& getEntityType(const GameState& state) const;
		const std::unordered_set<EntityTypeID>& getSpawneableEntities(const GameState& state) const;

		//RAW Values
		Vector2f getPosition(const GameState& state) const;
		int getTechnologyID()const
		{
			return data.technologyID;
		}
		int getPlayerID() const;
		int getPlayerID(const GameState& state) const;
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

		// Check if not equal
		bool operator !=(const ActionTarget& other)
		{
			if (targetType != other.targetType)
				return false;
			else
			{
				switch (targetType)
				{
				case Position:

					//if (static_cast<int>(data.position.x) == static_cast<int>(other.data.position.x) && static_cast<int>(data.position.y) == static_cast<int>(other.data.position.y))
					
					//return (static_cast<int>(data.position.x) == static_cast<int>(other.data.position.x) && static_cast<int>(data.position.y) == static_cast<int>(other.data.position.y));
					
					return Vector2i(data.position.x, data.position.y) != Vector2i(other.data.position.x, other.data.position.y);
					break;
				case EntityReference:
					return data.entityID != other.data.entityID;
					break;
				case PlayerReference:
					return data.playerID != other.data.playerID;
					break;
				case EntityTypeReference:
					return data.entityTypeID != other.data.entityTypeID;
					break;
				case TechnologyReference:
					return data.technologyID != other.data.technologyID;
					break;
				case ContinuousActionReference:
					return data.continuousActionID != other.data.continuousActionID;
					break;
				}
			}
		}

	private:
		union Data
		{
			Vector2f position;
			int entityID;
			int playerID;
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
