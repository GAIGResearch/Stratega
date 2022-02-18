#pragma once
#include <map>
#include <string>
#include <Stratega/Agent/Agent.h>
#include <Stratega/Representation/EntityPlacement.h>
#include <Stratega/Configuration/FunctionParser.h>
#include <Stratega/Configuration/RenderConfig.h>
#include <Stratega/Representation/TechnologyTree.h>
#include <Stratega/Representation/GameDescription.h>
#include <Stratega/Representation/LevelDefinition.h>
#include <Stratega/Representation/BuffType.h>
#include <yaml-cpp/yaml.h>
#include <memory>

namespace SGA
{	
    struct GameConfig
    {
        // Rendering - ToDo Split renderConfig and move into dedicated parts (Tile, Entity, etc)
        std::unique_ptr<RenderConfig> renderConfig;
    	
    	// Game information
        GameType gameType = GameType::TBS;

        int tickLimit = 100;
        int numPlayers = -1;

        bool applyFogOfWar=true;
    	
    	// Other stuff
        std::vector<std::pair<std::string, YAML::Node>> agentParams;
        std::unordered_map<int, TileType> tileTypes;

    	//ForwardModel
        std::unique_ptr<ForwardModel> forwardModel;
    	
    	// Players
        std::unordered_map<ParameterID, Parameter> playerParameterTypes;
        std::unordered_set<EntityTypeID> playerSpawnableTypes;
        std::vector<int> playerActionIds;
    	// Entities
        std::unordered_map<std::string, ParameterID> parameters;
        std::unordered_map<std::string, std::unordered_set<int>> entityGroups;
        std::unordered_map<int, EntityType> entityTypes;
        //Gamestate
        std::unordered_map<ParameterID, Parameter> stateParameterTypes;
        //Buffs
        std::unordered_map<int, BuffType> buffsTypes;
    	// Actions
        std::unordered_map<int, ActionType> actionTypes;
    	// State-Generation
        //std::string boardString;
        int defaultTileTypeID=-1;
        std::unordered_map<int, LevelDefinition> levelDefinitions;
        int selectedLevel;

    	// Technology tree
        TechnologyTreeCollection technologyTreeCollection;
            	
        virtual std::vector<std::unique_ptr<Agent>> generateAgents() const;
        virtual std::unique_ptr<GameState> generateGameState(int levelID=-1) const;

        virtual void initializeGamestate(GameState& state) const
        {
            state.setGameType(gameType);
            state.setTickLimit(tickLimit);
            state.setCurrentTBSPlayer(gameType == SGA::GameType::RTS ? -1 : 0);
        }

        virtual void generateGameInfo(GameState& state)const
        {
            //GameInfo
            GameInfo gameInfo;
            gameInfo.setEntityTypes(std::make_shared<std::unordered_map<int, EntityType>>(entityTypes));
            gameInfo.setTileTypes(std::make_shared<std::unordered_map<int, TileType>>(tileTypes));
            gameInfo.setPlayerParameterTypes(std::make_shared<std::unordered_map<ParameterID, Parameter>>(playerParameterTypes));
            gameInfo.setStateParameterTypes(std::make_shared<std::unordered_map<ParameterID, Parameter>>(stateParameterTypes));
            gameInfo.setEntityGroups(entityGroups);
            gameInfo.setActionTypes(std::make_shared<std::unordered_map<int, ActionType>>(actionTypes));
            gameInfo.setBuffTypes(std::make_shared<std::unordered_map<int, BuffType>>(buffsTypes));
            gameInfo.setParameterIDLookup(std::make_shared<std::unordered_map<std::string, ParameterID>>(parameters));
            gameInfo.setTechnologyTreeCollection(std::make_shared<TechnologyTreeCollection>(technologyTreeCollection));
            gameInfo.setPlayerSpawnableTypes(std::make_shared<std::unordered_set<EntityTypeID>>(playerSpawnableTypes));
            gameInfo.setYAMLPath(yamlPath);
            gameInfo.setGameDescription(std::make_shared<GameDescription>(actionCategories, entityCategories));
            state.setGameInfo(std::make_shared<GameInfo>(gameInfo));
        }

        virtual void addPlayers(GameState& state) const
        {
            std::unordered_set<int> playerIDs;
            for (size_t i = 0; i < getNumberOfPlayers(); i++)
            {
                playerIDs.emplace(addPlayer(state, *state.getGameInfo()));
            }
        }

        virtual void createTileLookup(GameState& state) const
        {
            std::unordered_map<char, const TileType*> tileLookup;
            // error: variable ‘defaultTile?set but not used [-Werror=unused-but-set-variable]
            //const auto* defaultTile = &state.getGameInfo()->getTileTypes().begin()->second;
            const auto& newTileTypes = state.getGameInfo()->getTileTypes();
            for (const auto& idTilePair : newTileTypes)
            {
                tileLookup.emplace(idTilePair.second.getSymbol(), &idTilePair.second);
                /*if (idTilePair.second.isDefaultTile())
                {
                    defaultTile = &idTilePair.second;
                    defaultTileTypeID = defaultTile->getID();
                }*/
                    
            }
        }

        virtual  void createEntitiesLookup(GameState& state) const
        {
            std::unordered_map<char, const EntityType*> entityLookup;
            for (const auto& idEntityPair : state.getGameInfo()->getEntityTypes())
            {
                entityLookup.emplace(idEntityPair.second.getSymbol(), &idEntityPair.second);
            }
        }

        std::vector<Tile> instanceTiles(GameState& /*state*/, const Grid2D<std::shared_ptr<TileType>>& board) const
        {           
            // Configure board and spawn entities
            std::vector<Tile> tiles;
            //Instance Tiles
            for (size_t y = 0; y < board.getHeight(); y++)
            {
                for (size_t x = 0; x < board.getWidth(); x++)
                {
                    tiles.emplace_back(board.get(static_cast<int>(x), static_cast<int>(y))->toTile(static_cast<int>(x), static_cast<int>(y)));
                }
            } 
            return tiles;
        }

        virtual void instanceEntities(GameState& state, const std::vector<EntityPlacement> &entityPlacements) const
        {           
            //Instance Entities
            for (auto& entity : entityPlacements)
            {
                state.addEntity(*entity.entityType, entity.ownerID, entity.position);
            }       
        }

        virtual void initBoard(GameState& state, std::vector<Tile>& tiles, const Grid2D<std::shared_ptr<TileType>>& board) const
        {
            //Initialize board with size and set of tiles.
            state.initBoard(static_cast<int>(board.getWidth()), tiles);
        }

        virtual void initResearchTechs(GameState& state) const
        {
            //Initialize researched list for all players
            state.initResearchTechs();
        }

    	//ActionCategories
        std::unordered_map<ActionCategory, std::vector<int>> actionCategories;

        //EntityCategories
        std::unordered_map<EntityCategory, std::vector<int>> entityCategories;

    	//Yaml path
        std::string yamlPath;
        
    	//Origin resources path
        std::string resourcesPath;


        //Game runner config
        //Computation budget time
        bool shouldCheckComputationTime = false;
        long budgetTimeMs = 40;
        long disqualificationBudgetTimeMs = 60;
        int maxNumberWarnings = 3;

        //Intialization budget time
        bool shouldCheckInitTime = false;
        long initBudgetTimetMs = 40;
        long initDisqualificationBudgetTimeMs = 60;

    	//Base utilities
        size_t getNumberOfPlayers() const;
        int getEntityID(const std::string& name) const;
        int getTileID(const std::string& name) const;
        int getActionID(const std::string& name) const;
        int getTechnologyID(const std::string& name) const;

        //Adds a new player to the game.
        virtual int addPlayer(GameState& state, GameInfo& gameInfo) const;

        virtual ~GameConfig()=default;
    };
}
