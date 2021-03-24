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

		GameDescription& getGameDescription() const;
		
		//Entities
		const EntityType& getEntityType(int entityTypeID) const;

		int getParameterGlobalID(std::string parameterName);

		const Parameter& getParameterType(int entityTypeID, int globalParameterID) const;

		bool checkEntityHaveParameter(int entityTypeID, const std::string& parameterName) const;
		
		//Actions
		const ActionType& getActionTypeConst(int actionTypeID);

		const ActionType& getActionType(int typeID) const;		

		int getActionTypeID(std::string parameterName);

		//Player
		const SGA::Parameter& getPlayerParameter(ParameterID id) const;		

		//Tiles
		const TileType& getTileType(int tileTypeID) const;		
	};
}
