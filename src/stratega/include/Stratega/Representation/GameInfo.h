#pragma once
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <Stratega/Representation/GameDescription.h>
#include <memory>
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

	private:

		/// <summary>
		/// Path to game YAML file
		/// </summary>
		std::string yamlPath;

		/// <summary>
		/// Pointer to the description of the game.
		/// </summary>
		std::shared_ptr <GameDescription> gameDescription;

		/// <summary>
		/// Relation of parameters defined in the game: name -> id
		/// </summary>
		std::shared_ptr<std::unordered_map<std::string, ParameterID>> parameterIDLookup;

		/// <summary>
		/// Relation of parameters defined in the game: id -> parameter object.
		/// </summary>
		std::shared_ptr<std::unordered_map<ParameterID, Parameter>> playerParameterTypes;

		/// <summary>
		/// Relation of parameters defined in the game: id -> parameter object.
		/// </summary>
		std::shared_ptr<std::unordered_map<ParameterID, Parameter>> stateParameterTypes;

		/// <summary>
		/// Set of entity type IDs of this game.
		/// </summary>
		std::shared_ptr<std::unordered_set<EntityTypeID>> playerSpawnableTypes;

		/// <summary>
		/// Relation of entity types in this game: id -> entity type object
		/// </summary>
		std::shared_ptr<std::unordered_map<int, EntityType>> entityTypes;

		/// <summary>
		/// Relation of action types in this game: id -> action type object
		/// </summary>
		std::shared_ptr<std::unordered_map<int, ActionType>> actionTypes;

		/// <summary>
		/// Relation of buff types in this game: id -> buff type object
		/// </summary>
		std::shared_ptr<std::unordered_map<int, BuffType>> buffTypes;

		/// <summary>
		/// Relation of tile types in this game: id -> tile type object
		/// </summary>
		std::shared_ptr<std::unordered_map<int, TileType>> tileTypes;

		/// <summary>
		/// Pointer to the technology tree for this game.
		/// </summary>
		std::shared_ptr<TechnologyTreeCollection> technologyTreeCollection;

		/// <summary>
		/// Relation of entity types in the Entity Groups of this game: name (of entity group) -> set<entity type id>
		/// </summary>
		std::unordered_map<std::string, std::unordered_set<EntityTypeID>> entityGroups;

	public:

		/// <summary>
		/// Returns the entity type.
		/// </summary>
		/// <param name="entityTypeID">The ID of the entity type.</param>
		/// <returns>The entity type.</returns>
		const EntityType& getEntityType(int entityTypeID) const;

		/// <summary>
		/// Returns the buff type.
		/// </summary>
		/// <param name="buffTypeID">The ID of the buff type.</param>
		/// <returns>The buff type.</returns>
		const BuffType& getBuffType(int buffTypeID) const;

		/// <summary>
		/// Returns the tile type.
		/// </summary>
		/// <param name="tileTypeID">The ID of the tile type</param>
		/// <returns> The TileType.</returns>
		const TileType& getTileType(int tileTypeID) const;


		/**** PARAMETERS ***/

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
				
		/// <summary>
		/// Returns the parameter of the player.
		/// </summary>
		/// <param name="ParameterID">The ID of the parameter</param>
		/// <returns> The parameter.</returns>
		const SGA::Parameter& getPlayerParameter(ParameterID id) const;

		/// <summary>
		/// Returns the parameter of the player.
		/// </summary>
		/// <param name="ParameterID">The ID of the parameter</param>
		/// <returns> The parameter.</returns>
		const SGA::Parameter& getPlayerParameter(std::string& name) const;
		
		/// <summary>
		/// Returns the parameter of the player by index.
		/// </summary>
		/// <param name="ParameterID">The index of the parameter</param>
		/// <returns> The parameter.</returns>
		const SGA::Parameter& getPlayerParameterByIndex(int index) const;
		
		/// <summary>
		/// Returns the parameter of the state.
		/// </summary>
		/// <param name="ParameterID">The ID of the parameter</param>
		/// <returns> The parameter.</returns>
		const SGA::Parameter& getStateParameter(ParameterID id) const;

		/// <summary>
		/// Returns the parameter of the state.
		/// </summary>
		/// <param name="ParameterID">The ID of the parameter</param>
		/// <returns> The parameter.</returns>
		const SGA::Parameter& getStateParameter(std::string& name) const;
		
		/// <summary>
		/// Returns the parameter of the state by index.
		/// </summary>
		/// <param name="ParameterID">The index of the parameter</param>
		/// <returns> The parameter.</returns>
		const SGA::Parameter& getStateParameterByIndex(int index) const;


		/*** ACTION TYPES ***/

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


		/*** TECHNOLOGIES ***/

		/// <summary>
		/// Returns a map of tuples with the tree ID and how many technologies each has.
		/// </summary>
		/// <returns>A map with trees and technology counts</returns>
		std::unordered_map<int, int> getTechnologyCounts() const;

		/// <summary>
		/// Returns a technology node with an ID corresponding to the one passed by parameter. The node could come from
		/// any of the trees of the tree collection.
		/// </summary>
		/// <param name="technologyID">ID of the technology to search for.</param>
		/// <returns>The tree node searched for, an exception if it doesn't exist.</returns>
		const TechnologyTreeNode& getTechnology(int technologyID) const;

		/// <summary>
		/// Returns the ID of a technology that matches the requested name, in any of the trees of the tree collection.
		/// </summary>
		/// <param name="technologyName">Name of the technology to search for.</param>
		/// <returns>ID of the technology searched for, or a runtime error if it doesn't exist.</returns>
		int getTechnologyTypeID(const std::string& technologyName) const;

		/// <summary>
		/// Returns a list with all technologies of a given tree, specified by its ID
		/// </summary>
		/// <param name="techTreeID">ID of the tech tree to look for</param>
		/// <returns>A list with all technologies of a given research tree</returns>
		std::vector<TechnologyTreeNode> getTreeNodes(int techTreeID) const;

		/// <summary>
		/// Returns the IDs of the tech trees of this game.
		/// </summary>
		/// <returns>Returns the IDs of the tech trees of this game.</returns>
		std::vector<int> getTechTreesIDs() const; 


		/* Setters for the members of this struct.*/
		void setYAMLPath(const std::string& path) { yamlPath = path; }
		void setGameDescription(std::shared_ptr <GameDescription> gd) { gameDescription = gd; }
		void setParameterIDLookup(std::shared_ptr<std::unordered_map<std::string, ParameterID>> pID) { parameterIDLookup = pID; }
		void setPlayerParameterTypes(std::shared_ptr<std::unordered_map<ParameterID, Parameter>> pt) { playerParameterTypes = pt; }
		void setStateParameterTypes(std::shared_ptr<std::unordered_map<ParameterID, Parameter>> pt) { stateParameterTypes = pt; }
		void setPlayerSpawnableTypes(std::shared_ptr<std::unordered_set<EntityTypeID>> pt) { playerSpawnableTypes = pt; }
		void setEntityTypes(std::shared_ptr<std::unordered_map<int, EntityType>> et) { entityTypes = et; }
		void setActionTypes(std::shared_ptr<std::unordered_map<int, ActionType>> at) { actionTypes = at; }
		void setBuffTypes(std::shared_ptr<std::unordered_map<int, BuffType>> bt) { buffTypes = bt; }
		void setTileTypes(std::shared_ptr<std::unordered_map<int, TileType>> tt) { tileTypes = tt; }
		void setTechnologyTreeCollection(std::shared_ptr<TechnologyTreeCollection> tt) { technologyTreeCollection = tt; }
		void setEntityGroups(std::unordered_map<std::string, std::unordered_set<EntityTypeID>> eg) { entityGroups = eg; }


		/* Getters for the members of this struct */
		const std::string& getYAMLPath() const { return yamlPath; }
		const GameDescription& getGameDescription() const { return *gameDescription; }
		const std::unordered_map<std::string, ParameterID>& getParameterIDLookup() const { return *parameterIDLookup; }
		const std::unordered_map<ParameterID, Parameter>& getPlayerParameterTypes() const { return *playerParameterTypes; }
		const std::unordered_map<ParameterID, Parameter>& getStateParameterTypes() const { return *stateParameterTypes; }
		const std::unordered_set<EntityTypeID>& getPlayerSpawnableTypes() const { return *playerSpawnableTypes; }
		const std::unordered_map<int, EntityType>& getEntityTypes() const { return *entityTypes; }
		const std::unordered_map<int, ActionType>& getActionTypes() const { return *actionTypes; }
		const std::unordered_map<int, BuffType>& getBuffTypes() const { return *buffTypes; }
		const std::unordered_map<int, TileType>& getTileTypes() const { return *tileTypes; }
		const TechnologyTreeCollection& getTechnologyTreeCollection() const { return *technologyTreeCollection; }
		const std::unordered_map<std::string, std::unordered_set<EntityTypeID>>& getEntityGroups() const { return entityGroups; }

	};

}
