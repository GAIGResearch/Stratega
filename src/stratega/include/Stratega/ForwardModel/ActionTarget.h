#pragma once
#include <string>
#include <unordered_set>
#include <Stratega/Representation/Vector2.h>

#include "Stratega/Representation/TileType.h"


namespace SGA
{
	struct GameState;
	struct Entity;
	struct Player;
	struct EntityType;
	typedef int EntityTypeID;
	typedef int TileTypeID;

	struct ActionType;

	/// <summary>
	/// Used by <see cref="SGA::Action"/> to reference any possible type of target available in Stratega.
	/// It contains a union of different types of data.
	/// </summary>
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
			ContinuousActionReference,
			TileTypeReference,
			Gamestate
		};
		
		static ActionTarget createPositionActionTarget(Vector2f position);
		static ActionTarget createEntityActionTarget(int entityID);
		static ActionTarget createPlayerActionTarget(int playerID);
		static ActionTarget createGameStateActionTarget();
		static ActionTarget createTechnologyEntityActionTarget(int technologyID);
		static ActionTarget createContinuousActionActionTarget(int continuousActionID);
		static ActionTarget createEntityTypeActionTarget(EntityTypeID entityTypeID);
		static ActionTarget createTileTypeActionTarget(TileTypeID entityTypeID);


		//References		
		const Entity& getEntityConst(const GameState& state) const;
		Entity* getEntity(GameState& state) const;
		Player& getPlayer(GameState& state) const;
		const Player& getPlayerConst(const GameState& state) const;
		const EntityType& getEntityType(const GameState& state) const;
		const TileType& getTileType(const GameState& state) const;
		const std::unordered_set<EntityTypeID>& getSpawnableEntities(const GameState& state) const;

		//RAW Values
		Vector2f getPosition(const GameState& state) const;
		Vector2f getPosition() const;
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
		std::string getTypeString() const
		{
			switch (targetType)
			{
			case EntityReference:
				return "EntityReference";
				break;
			case Position:
				return "Position";
				break;
			case PlayerReference:
				return "PlayerReference";
				break;
			case EntityTypeReference:
				return "EntityTypeReference";
				break;
			case TechnologyReference:
				return "TechnologyReference";
				break;
			case ContinuousActionReference:
				return "ContinuousActionReference";
				break;
			case TileTypeReference:
				return "TileTypeReference";
				break;
			case Gamestate:
				return "Gamestate";
				break;
			default:
				return "Not defined";
			}
		}
		std::string getValueString(const GameState& state) const;
		//Check if this action target is valid in the received gamestate
		static bool isValidWithTargets(const GameState& state,const ActionType& actionType ,const std::vector<ActionTarget>& actionTargets);

		//Check if action target is valid in the received gamestate
		bool isValid(const GameState& state) const;		

	private:
		union Data
		{
			Vector2f position;
			int entityID;
			int playerID;
			EntityTypeID entityTypeID;
			TileTypeID tileTypeID;
			int technologyID;
			int continuousActionID;

			Data(Vector2f data):
				position(data)
			{
			}

			Data()
			{

			}

			Data(ActionTarget::Type targetType, int data):
				position(SGA::Vector2f(-1,-1))
			{
				switch (targetType)
				{
				case EntityReference:
					entityID = data;
					break;
				case PlayerReference:
					playerID = data;
					break;
				case EntityTypeReference:
					entityTypeID = data;
					break;
				case TechnologyReference:
					technologyID = data;
					break;
				case ContinuousActionReference:
					continuousActionID = data;
					break;
				case TileTypeReference:
					tileTypeID = data;
					break;
				case Position:
					position = SGA::Vector2f(-1, -1);
					break;
				default:
					break;
				}
			}
		};

		Type targetType;
		Data data;

		// Private since this class should only be constructed using the static methods
		ActionTarget(const Type& type, const Data& newData);
	};

}
