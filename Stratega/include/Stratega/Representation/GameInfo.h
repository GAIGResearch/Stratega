#pragma once
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <Stratega/Representation/GameDescription.h>

namespace SGA
{
	struct EntityType;
	struct ActionType;
	struct TileType;
	class TechnologyTreeCollection;
	typedef int ParameterID;
	struct Parameter;
	typedef int EntityTypeID;

	/// <summary>
	/// Contains all the type information used by the game.
	/// It have different methods to get the different types: parameter, entity, action...
	/// The agent can access to the yaml path where the game configuration is defined.
	/// </summary>
	struct GameInfo
	{
		//Yaml
		std::string yamlPath;
		
        //Type information
        std::shared_ptr<std::unordered_map<std::string, ParameterID>> parameterIDLookup;
        std::shared_ptr<std::unordered_map<ParameterID, Parameter>> playerParameterTypes;
        std::shared_ptr<std::unordered_set<EntityTypeID>> playerSpawnableTypes;
        std::shared_ptr<std::unordered_map<int, EntityType>> entityTypes;
        std::shared_ptr<std::unordered_map<int, ActionType>> actionTypes;
        std::shared_ptr<std::unordered_map<int, TileType>> tileTypes;

        //Technology tree
        std::shared_ptr<TechnologyTreeCollection> technologyTreeCollection;
        std::unordered_map<std::string, std::unordered_set<EntityTypeID>> entityGroups;

		//GameDescription
		std::shared_ptr <GameDescription> gameDescription;

		/// <summary>
		/// Returns the game description of the game.
		/// </summary>
		/// <returns>GameDescription.</returns>
		GameDescription& getGameDescription() const;
		
		//Entities
		
		/// <summary>
		/// Returns the entity type.
		/// </summary>
		/// <param name="entityTypeID">The ID of the entity type.</param>
		/// <returns>The entity type.</returns>
		const EntityType& getEntityType(int entityTypeID) const;

		/// <summary>
		/// Returns the ID of the parameter.
		/// </summary>
		/// <param name="parameterName">The parameter name.</param>
		/// <returns>The parameter ID.</returns>
		int getParameterGlobalID(std::string parameterName);


		/// <summary>
		/// Returns the parameter type from a entityType.
		/// </summary>
		/// <param name="entityTypeID">The entityType ID.</param>
		/// <param name="globalParameterID">The parameter ID.</param>
		/// <returns>The parameter.</returns>
		const Parameter& getParameterType(int entityTypeID, int globalParameterID) const;


		/// <summary>
		/// Returns true if the entity type has the searched parameter
		/// </summary>
		/// <param name="entityTypeID">The entity type.</param>
		/// <param name="parameterName">The parameter name.</param>
		/// <returns>True if it has the parameter</returns>
		bool checkEntityHaveParameter(int entityTypeID, const std::string& parameterName) const;

		
		//Actions
		/// <summary>
		/// Returns the action type.
		/// </summary>
		/// <param name="actionTypeID">The ID of the action type</param>
		/// <returns> The action type.</returns>
		const ActionType& getActionTypeConst(int actionTypeID);

		/// <summary>
		/// Returns the action type.
		/// </summary>
		/// <param name="typeID">The ID of the action type</param>
		/// <returns> The action type.</returns>
		const ActionType& getActionType(int typeID) const;		

		/// <summary>
		/// Returns the action type ID.
		/// </summary>
		/// <param name="actionName">The name of the action type</param>
		/// <returns> The action type ID.</returns>
		int getActionTypeID(std::string actionName);

		//Player
		/// <summary>
		/// Returns the parameter of the player.
		/// </summary>
		/// <param name="ParameterID">The ID of the parameter</param>
		/// <returns> The parameter.</returns>
		const SGA::Parameter& getPlayerParameter(ParameterID id) const;		

		//Tiles
		/// <summary>
		/// Returns the tile type.
		/// </summary>
		/// <param name="tileTypeID">The ID of the tile type</param>
		/// <returns> The TileType.</returns>
		const TileType& getTileType(int tileTypeID) const;		
	};
}
