//#include <filesystem>
#include <Stratega/Configuration/GameConfigParser.h>
#include <Stratega/Agent/AgentFactory.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/ForwardModel/RTSForwardModel.h>
#include <yaml-cpp/yaml.h>
#include <fstream>


namespace SGA
{
    std::unique_ptr<GameConfig> loadConfigFromYAML(const std::string& filePath, const std::string& resourcesPath)
    {
        GameConfigParser parser;
        return parser.parseFromFile(filePath,resourcesPath);
    }

	std::unordered_map<int, LevelDefinition> loadLevelsFromYAML(const std::string& fileMapsPath, const GameConfig& config)
	{        
        GameConfigParser parser;
        return parser.parseLevelsFromFile(fileMapsPath, config);
	}

	std::unordered_map<int, LevelDefinition> GameConfigParser::parseLevelsFromFile(const std::string& fileMapsPath, const GameConfig& config) const
	{
        std::unordered_map<int, LevelDefinition> levelDefinitions;
    	
        //Load maps from a different path
        //Check if is path        
        std::ifstream file(fileMapsPath);
        if (file)
        {
            std::cout << "The maps file exist" << std::endl;

            //Lets read the yaml file
            auto mapsConfig = YAML::LoadFile(fileMapsPath);

            //Read maps
            if (mapsConfig["Maps"].IsDefined())
            {
                parseMaps(mapsConfig, levelDefinitions, config);
            }
            else
            {
                throw std::runtime_error("Cannot find definition for the maps");
            }
        }

        return levelDefinitions;
	}

	std::unique_ptr<GameConfig> GameConfigParser::parseFromFile(const std::string& filePath, const std::string& resourcesPath) const
	{
		auto configNode = YAML::LoadFile(filePath);
        auto config = std::make_unique<GameConfig>();
        config->resourcesPath = resourcesPath;
        config->yamlPath = filePath;

        auto gameConfigNode = loadNode(configNode, "GameConfig", *config);
        config->gameType = gameConfigNode["Type"].as<GameType>();
        config->tickLimit = gameConfigNode["RoundLimit"].as<int>(config->tickLimit);
        config->numPlayers = gameConfigNode["PlayerCount"].as<int>(config->numPlayers);

        config->applyFogOfWar = gameConfigNode["FogOfWar"].as<bool>(config->applyFogOfWar);

        //Parse parameters
        auto parametersNode = gameConfigNode["Parameters"];
        parseParameterList(parametersNode, *config, config->stateParameterTypes);

		// Parse complex structures
		// Order is important, only change if you are sure that a function doesn't depend on something parsed before it
		parseEntities(loadNode(configNode, "Entities", *config), *config);
        parseEntityGroups(loadNode(configNode, "EntityGroups", *config), *config);
        parseAgents(loadNode(configNode, "Agents", *config), *config);
        parseTileTypes(loadNode(configNode, "Tiles", *config), *config);

        parsePlayers(loadNode(configNode, "Player", *config), *config);

        if(loadNode(configNode, "Buffs", *config).IsDefined())
           parseBuffs(loadNode(configNode, "Buffs", *config), *config);
        

		if(loadNode(configNode, "TechnologyTrees", *config).IsDefined())
			parseTechnologyTrees(loadNode(configNode, "TechnologyTrees", *config), *config);
		
        parseActions(loadNode(configNode, "Actions", *config), *config);
        parseForwardModel(loadNode(configNode, "ForwardModel", *config), *config);

        if (loadNode(configNode, "GameDescription", *config).IsDefined())
            parseGameDescription(loadNode(configNode, "GameDescription", *config), *config);

        if (loadNode(configNode, "GameRunner", *config).IsDefined())
            parseGameRunner(loadNode(configNode, "GameRunner", *config), *config);

        assignPlayerActions(loadNode(configNode, "Player", *config), *config);
        assignEntitiesActions(loadNode(configNode, "Entities", *config), *config);

    	// Parse render data - ToDo split into the dedicated functions (Entity, Tile, etc)
        parseRenderConfig(configNode, *config);

    	//Parse last the boards after adding the actions to entities
        parseBoardGenerator(loadNode(configNode, "Board", *config), *config);
        return config;
	}

	void GameConfigParser::parseAgents(const YAML::Node& agentsNode, GameConfig& config) const
	{
        if(!agentsNode.IsDefined())
        {
            throw std::runtime_error("Cannot find definition for Agents");
        }

        for (const auto& agentNode : agentsNode)
        {
            if (agentNode.IsScalar())
            {
                config.agentParams.emplace_back(agentNode.as<std::string>(), YAML::Null);
            }
            else
            {
                auto map = agentNode.as<std::map<std::string, YAML::Node>>();
                const auto& firstEntry = *map.begin();
                config.agentParams.emplace_back(firstEntry);
            }
        }
	}

    void GameConfigParser::parseTileTypes(const YAML::Node& tilesNode, GameConfig& config) const
	{
        if(!tilesNode.IsDefined())
        {
            throw std::runtime_error("Cannot find definition for Tiles");
        }
		
        auto tileConfigs = tilesNode.as<std::map<std::string, YAML::Node>>();
        std::unordered_map<std::string, TileType> types;
        auto idCounter = -1;

        //Add fog of war tile
        TileType fogOfWarType;
        fogOfWarType.setID(idCounter++);
        fogOfWarType.setName("FogOfWar");
        fogOfWarType.setWalkable(false);
        fogOfWarType.setSymbol('_');
        config.tileTypes.emplace(fogOfWarType.getID(), std::move(fogOfWarType));

		for(const auto& nameConfigPair : tileConfigs)
		{
            TileType type;
            type.setID(idCounter++);
            type.setName(nameConfigPair.first);
			type.setWalkable(nameConfigPair.second["IsWalkable"].as<bool>(type.isWalkable()));
			type.setBlockSight(nameConfigPair.second["BlocksSight"].as<bool>(type.blockSight()));
            type.setDefaultTile(nameConfigPair.second["DefaultTile"].as<bool>(false));
			type.setSymbol(nameConfigPair.second["Symbol"].as<char>('-'));
            config.tileTypes.emplace(type.getID(), std::move(type));
		}
	}

    void GameConfigParser::parseBoardGenerator(const YAML::Node& boardNode, GameConfig& config) const
	{
		if(!boardNode.IsDefined())
		{
            throw std::runtime_error("Cannot find definition for the Board");
		}
    	
        std::unordered_map<int, LevelDefinition> levelDefinitions;
    	
        //Load maps from file    	
        if (boardNode["GenerationType"].as<std::string>() == "Manual")
        {
            

            //Check if user has defined multiple maps
            if (boardNode["Maps"].IsDefined())
            {
                if (boardNode["Maps"].IsScalar())
                {
                    auto path = boardNode["Maps"].as<std::string>();

                    //Check if is path
                    std::ifstream file(path);
                    if (file)
                    {
                        std::cout << "The file exist" << std::endl;

                        //Lets read the yaml file
                        auto mapsConfig = YAML::LoadFile(path);

                        //Read maps
                        if (mapsConfig["Maps"].IsDefined())
                        {
                            parseMaps(mapsConfig, levelDefinitions, config);
                        }
                        else
                        {
                            throw std::runtime_error("Cannot find definition for the maps");
                        }
                    }
                }
                else
                {
                    parseMaps(boardNode, levelDefinitions, config);
                }
            }

            std::string boardString = boardNode["Layout"].as<std::string>();
            //Check if user choose one from the loaded maps
            bool found = false;
            int indexMap = 0;
            for (auto& levelDefinition : levelDefinitions)
            {
                if (levelDefinition.second.name == boardString)
                {
                    found = true;
                    config.selectedLevel = indexMap;
                }
                indexMap++;
            }

            if (!found)
            {
                //Parse definition
                parseLevelDefinition(boardNode["Layout"], boardString, levelDefinitions, config);
                config.selectedLevel = static_cast<int>(levelDefinitions.size()) - 1;
            }

            config.levelDefinitions = levelDefinitions;
        }
        else
        {
            throw std::runtime_error("Unknown board-generation type " + boardNode["GenerationType"].as<std::string>());
        }
    	
       

	}

    void GameConfigParser::parseEntities(const YAML::Node& entitiesNode, GameConfig& config) const
    {
        if(!entitiesNode.IsDefined())
        {
            throw std::runtime_error("Cannot find definition for Entities");
        }
		
        auto types = entitiesNode.as<std::map<std::string, YAML::Node>>();
        for (const auto& nameTypePair : types)
        {
            EntityType type;
            type.setName(nameTypePair.first);
            type.setSymbol(nameTypePair.second["Symbol"].as<char>('\0'));
            type.setID(static_cast<int>(config.entityTypes.size()));
            
            
            if (!nameTypePair.second["LineOfSightRange"].IsDefined())
                type.setLoSRange(0);
            else
                type.setLoSRange(nameTypePair.second["LineOfSightRange"].as<double>());
            
            if (nameTypePair.second["Parameters"].IsDefined())
                parseParameterList(nameTypePair.second["Parameters"], config, type.getParameters());

            //type.continuousActionTime = nameTypePair.second["Time"].as<int>(0);
            config.entityTypes.emplace(type.getID(), std::move(type));
        }
    }

	void GameConfigParser::parseEntityGroups(const YAML::Node& entityGroupsNode, GameConfig& config) const
    {
        if (entityGroupsNode.IsDefined())
        {
            auto groups = entityGroupsNode.as<std::map<std::string, std::vector<std::string>>>();
            for (const auto& nameGroupPair : groups)
            {
                config.entityGroups.emplace(nameGroupPair.first, std::unordered_set<int>());
                for (const auto& entityName : nameGroupPair.second)
                {
                    config.entityGroups[nameGroupPair.first].emplace(config.getEntityID(entityName));
                }
            }
        }

        // Predefined groups
        config.entityGroups.emplace("All", std::unordered_set<int>());
        for (const auto& idEntityPair : config.entityTypes)
        {
            // Group that contains all entities
            config.entityGroups.at("All").emplace(idEntityPair.first);
        	// Group that contains one entity
            config.entityGroups.emplace(idEntityPair.second.getName(), std::initializer_list<int>{ idEntityPair.first });
        }
    }

    void GameConfigParser::parseActions(const YAML::Node& actionsNode, GameConfig& config) const
    {
        if(!actionsNode.IsDefined())
        {
            throw std::runtime_error("Cannot find definition for Actions");
        }

        FunctionParser parser;
        
        auto context = ParseContext::fromGameConfig(config);      
        for (const auto& nameTypePair : actionsNode.as<std::map<std::string, YAML::Node>>())
        {
            ActionType type;
            type.setID(static_cast<int>(config.actionTypes.size()));
            type.setName(nameTypePair.first);
        	
            context.targetIDs.emplace("Source", 0);
            //context.targetIDs.emplace("GameState", -1);
        	
        	//Parse all the targets
            if(nameTypePair.second["Targets"].IsDefined())
            for (auto& target : nameTypePair.second["Targets"].as<std::map<std::string, YAML::Node>>())
            {
                TargetType newTarget;
                context.targetIDs.emplace(target.first, static_cast<int>(context.targetIDs.size()));
                newTarget = parseTargetType(target.second, config);
            	
                std::vector<std::shared_ptr<Condition>> targetConditionsList;            	
                //// Parse target conditions
		        auto targetConditions = target.second["Conditions"].as<std::vector<std::string>>(std::vector<std::string>());
		        parser.parseFunctions(targetConditions, targetConditionsList, context);            	
                type.getTargets().emplace_back(newTarget, targetConditionsList);
            }
        	
            type.setSourceType(nameTypePair.second["Type"].as<ActionSourceType>());
            type.setCooldown(nameTypePair.second["Cooldown"].as<int>(0));
        	
            // Parse preconditions            
        	auto preconditions = nameTypePair.second["Preconditions"].as<std::vector<std::string>>(std::vector<std::string>());
            parser.parseFunctions(preconditions, type.getPreconditions(), context);

            // Parse effects
            auto effects = nameTypePair.second["Effects"].as<std::vector<std::string>>(std::vector<std::string>());
            parser.parseFunctions(effects, type.getEffects(), context);

            type.setContinuous(false);
        	
        	//Continuous Action Stuff
            if (nameTypePair.second["TriggerComplete"].IsDefined())
            {
                type.setContinuous(true);
                auto targetConditions = nameTypePair.second["TriggerComplete"].as<std::vector<std::string>>(std::vector<std::string>());
                parser.parseFunctions(targetConditions, type.getTriggerComplete(), context);
            }
        	
            if (nameTypePair.second["OnStart"].IsDefined())
            {
                type.setContinuous(true);

                auto effectStrings = nameTypePair.second["OnStart"].as<std::vector<std::string>>(std::vector<std::string>());
                parser.parseFunctions(effectStrings, type.getOnStart(), context);
            }

            if (nameTypePair.second["OnTick"].IsDefined())
            {
                type.setContinuous(true);

                auto effectStrings = nameTypePair.second["OnTick"].as<std::vector<std::string>>(std::vector<std::string>());
                parser.parseFunctions(effectStrings, type.getOnTick(), context);
            }

            if (nameTypePair.second["OnComplete"].IsDefined())
            {
                type.setContinuous(true);

                auto effectStrings = nameTypePair.second["OnComplete"].as<std::vector<std::string>>(std::vector<std::string>());
                parser.parseFunctions(effectStrings, type.getOnComplete(), context);
            }

            if (nameTypePair.second["OnAbort"].IsDefined())
            {
                type.setContinuous(true);

                auto effectStrings = nameTypePair.second["OnAbort"].as<std::vector<std::string>>(std::vector<std::string>());
                parser.parseFunctions(effectStrings, type.getOnAbort(), context);
            }
        	
            config.actionTypes.emplace(type.getID(), std::move(type));
            context.targetIDs.clear();
        }
    }

    TargetType GameConfigParser::parseTargetType(const YAML::Node& node, const GameConfig& config) const
    {
        TargetType targetType;
        targetType.setType(node["Type"].as<TargetType::Type>());
        if (targetType.getType() == TargetType::Position)
        {        	
        	targetType.setSamplingMethod(node["SamplingMethod"].as<std::shared_ptr<SamplingMethod>>());
        }
        else if (targetType == TargetType::Entity || targetType == TargetType::EntityType)
        {
        	if(targetType == TargetType::Entity)
				targetType.setSamplingMethod(node["SamplingMethod"].as<std::shared_ptr<SamplingMethod>>());
        	
            targetType.setGroupEntityTypes(parseEntityGroup(node["ValidTargets"], config));
        }
        else if (targetType.getType() == TargetType::Technology)
        {
            auto techNode = node["ValidTargets"];
        	if(techNode.IsScalar() && techNode.as<std::string>() == "All")
        	{
        		for(const auto& tree: config.technologyTreeCollection.technologyTreeTypes)
        		{
        			for(const auto& tech : tree.second.technologies)
						targetType.getTechnologyTypes().insert(tech.first);
        		}
        	}
            else if(techNode.IsSequence())
            {
                auto technologies = techNode.as<std::vector<std::string>>(std::vector<std::string>());
                //Assigne technology IDs to the technologytypes map
                for (auto& technology : technologies)
                {
                    targetType.getTechnologyTypes().insert(config.technologyTreeCollection.getTechnologyTypeID(technology));
                }
            }
        }
        return targetType;
    }

    ActionCategory GameConfigParser::parseActionCategory(const std::string& name) const
    {
        ActionCategory actionCategory;
              
        if (name == "Attack")        actionCategory = SGA::ActionCategory::Attack;
        else if (name == "Heal")     actionCategory = SGA::ActionCategory::Heal;
        else if (name == "Research") actionCategory = SGA::ActionCategory::Research;
        else if (name == "Gather")   actionCategory = SGA::ActionCategory::Gather;
        else if (name == "Move")     actionCategory = SGA::ActionCategory::Move;
        else if (name == "Spawn")    actionCategory = SGA::ActionCategory::Spawn;
        else if (name == "Empty")    actionCategory = SGA::ActionCategory::Empty;
        else throw std::runtime_error("Cannot find action category: " + name);
		
        return actionCategory;
    }

    EntityCategory GameConfigParser::parseEntityCategory(const std::string& name) const
    {
        EntityCategory entityCategory;

        if (name == "Base")           entityCategory = SGA::EntityCategory::Base;
        else if (name == "Building")  entityCategory = SGA::EntityCategory::Building;
        else if (name == "Spawner")   entityCategory = SGA::EntityCategory::Spawner;
        else if (name == "Unit")      entityCategory = SGA::EntityCategory::Unit;
        else if (name == "NoFighter") entityCategory = SGA::EntityCategory::NoFighter;
        else if (name == "Fighter")   entityCategory = SGA::EntityCategory::Fighter;
        else if (name == "Melee")     entityCategory = SGA::EntityCategory::Melee;
        else if (name == "Ranged")    entityCategory = SGA::EntityCategory::Ranged;
        else throw std::runtime_error("Cannot find entity category: " + name);

        return entityCategory;
    }

	void GameConfigParser::parseForwardModel(const YAML::Node& fmNode, GameConfig& config) const
	{
        if (!fmNode.IsDefined())
        {
            throw std::runtime_error("Cannot find a definition for the ForwardModel");
        }
		
        std::unique_ptr<ForwardModel> fm;
		if(config.gameType == GameType::TBS)
		{
            fm = std::make_unique<TBSForwardModel>();
		}
        else if(config.gameType == GameType::RTS)
        {
            fm = std::make_unique<RTSForwardModel>();
        }
       
        FunctionParser parser;
        auto context = ParseContext::fromGameConfig(config);
        context.targetIDs.emplace("Source", 0);
		
		//Parse Win Conditions
        auto winConditions = fmNode["WinConditions"].as<std::map<std::string, YAML::Node>>(std::map<std::string, YAML::Node>());
        
        for (auto& winCondition : winConditions)
        {
            std::vector<std::shared_ptr<Condition>> conditionList;
        	
            auto conditions = winCondition.second.as<std::vector<std::string>>(std::vector<std::string>());
            parser.parseFunctions<Condition>(conditions, conditionList, context);
            fm->addWinConditions(conditionList);
        }

        //Parse Lose Conditions
        auto loseConditions = fmNode["LoseConditions"].as<std::map<std::string, YAML::Node>>(std::map<std::string, YAML::Node>());

        for (auto& loseCondition : loseConditions)
        {
            std::vector<std::shared_ptr<Condition>> conditionList;

            auto conditions = loseCondition.second.as<std::vector<std::string>>(std::vector<std::string>());
            parser.parseFunctions<Condition>(conditions, conditionList, context);
            fm->addLoseConditions(conditionList);
        }
		
		// Parse Triggers
		for(const auto& trigger : fmNode["Trigger"].as<std::vector<YAML::Node>>(std::vector<YAML::Node>()))
		{
            auto map = trigger.as<std::map<std::string, YAML::Node>>();
			if(map.find("OnTick") != map.end())
			{
                auto nameEffectsPair = *map.find("OnTick");
				
                context.targetIDs.emplace("Source", 0);
                auto conditions = nameEffectsPair.second["Conditions"].as<std::vector<std::string>>(std::vector<std::string>());
                auto effects = nameEffectsPair.second["Effects"].as<std::vector<std::string>>(std::vector<std::string>());

                // Initiliaze OnTickEffect
                OnTickEffect onTickEffect;
                onTickEffect.type = nameEffectsPair.second["Type"].as<SourceOnTickEffectType>();

                if(onTickEffect.type== SourceOnTickEffectType::Entity)
                    onTickEffect.validTargets = parseEntityGroup(nameEffectsPair.second["ValidTargets"], config);

                parser.parseFunctions<Condition>(conditions, onTickEffect.conditions, context);
                parser.parseFunctions<Effect>(effects, onTickEffect.effects, context);
				// Add it to the fm
                fm->addOnTickEffect(onTickEffect);
			}
            else if(map.find("OnAdvance") != map.end())
			{
                auto nameEffectsPair = *map.find("OnAdvance");
				
                context.targetIDs.emplace("Source", 0);
                auto conditions = nameEffectsPair.second["Conditions"].as<std::vector<std::string>>(std::vector<std::string>());
                auto effects = nameEffectsPair.second["Effects"].as<std::vector<std::string>>(std::vector<std::string>());

                // Initiliaze OnTickEffect
                OnTickEffect onTickEffect;
                onTickEffect.type = nameEffectsPair.second["Type"].as<SourceOnTickEffectType>();

                if(onTickEffect.type== SourceOnTickEffectType::Entity)
                    onTickEffect.validTargets = parseEntityGroup(nameEffectsPair.second["ValidTargets"], config);

                parser.parseFunctions<Condition>(conditions, onTickEffect.conditions, context);
                parser.parseFunctions<Effect>(effects, onTickEffect.effects, context);
				// Add it to the fm
                fm->addOnAdvanceEffect(onTickEffect);
			}
            else if(map.find("OnSpawn") != map.end())
            {
                auto nameEffectsPair = *map.find("OnSpawn");
            	
                context.targetIDs.emplace("Source", 0);
                auto conditions = nameEffectsPair.second["Conditions"].as<std::vector<std::string>>(std::vector<std::string>());
                auto effects = nameEffectsPair.second["Effects"].as<std::vector<std::string>>(std::vector<std::string>());

                // Initiliaze OnTickEffect
                OnEntitySpawnEffect onSpawnEffect;                
                onSpawnEffect.validTargets = parseEntityGroup(nameEffectsPair.second["ValidTargets"], config);
                parser.parseFunctions<Condition>(conditions, onSpawnEffect.conditions, context);
                parser.parseFunctions<Effect>(effects, onSpawnEffect.effects, context);
                // Add it to the fm
                fm->addOnEntitySpawnEffect(onSpawnEffect);
            }
            else
            {
                //std::cerr << "Unknown trigger with name " << nameEffectsPair.first << " will be ignored." << std::endl;
            }

            context.targetIDs.clear();
		}
		
        config.forwardModel = std::move(fm);
	}

	void GameConfigParser::parseTechnologyTrees(const YAML::Node& techtreeNode, GameConfig& config) const
	{
        if (!techtreeNode.IsDefined())
        {
            throw std::runtime_error("Cannot find definition for Technology Trees");
        }
        int technologyNextID=0;
        auto types = techtreeNode.as<std::map<std::string, YAML::Node>>();
        for (const auto& nameTypePair : types)
        {
            SGA::TechnologyTreeType technologyTreeType;

            technologyTreeType.technologyTreeName = nameTypePair.first;
           
            for (const auto& nameTechPair : nameTypePair.second.as<std::map<std::string, YAML::Node>>())
            {
                TechnologyTreeNode newTechnology;

                newTechnology.id = technologyNextID++;
                newTechnology.name = nameTechPair.first;
				newTechnology.description= nameTechPair.second["Description"].as<std::string>();
                newTechnology.cost = parseCost(nameTechPair.second["Cost"], config);
                newTechnology.continuousActionTime = nameTechPair.second["Time"].as<int>(0);

                technologyTreeType.technologies[newTechnology.id]= newTechnology;
            }

            config.technologyTreeCollection.technologyTreeTypes[static_cast<int>(config.technologyTreeCollection.technologyTreeTypes.size())] = technologyTreeType;
        }


		//Go through all the tree types we have in the config
        for (auto& technologyTreeType : config.technologyTreeCollection.technologyTreeTypes)
        {
        	//Check it tehcnology in the treetype
            for (auto& technology : technologyTreeType.second.technologies)
            {

            	//Search the technology tree in the config yaml
                auto typeMap = techtreeNode.as<std::map<std::string, YAML::Node>>();
                auto techTreeTypeYaml = typeMap[technologyTreeType.second.technologyTreeName].as<std::map<std::string, YAML::Node>>();
                //Find the technology
                auto technologyYaml= techTreeTypeYaml[technology.second.name].as<std::map<std::string, YAML::Node>>();
            	//Get the parents of the technology
                auto parentsNames=technologyYaml["Requirements"].as<std::vector<std::string>>(std::vector<std::string>());

                for (auto& parent : parentsNames)
                {
                	//Get the new assigned technologyID of the parent and add it to the technology
                   technology.second.parentIDs.emplace_back( config.getTechnologyID(parent));
                }
            	
            }
        }
	}

    void GameConfigParser::parseGameDescription(const YAML::Node& gameDescription, GameConfig& config) const
    {
        //Parse Action Categories
        auto nodes = gameDescription["Actions"];
        auto types = nodes.as<std::map<std::string, YAML::Node>>();

        for (auto& type : types)
        {
        	// Parse action category
            auto category = parseActionCategory(type.first);

            // Assign actiontypes 
            std::vector<int> actionTypes;        	
            auto actions = type.second.as<std::vector<std::string>>(std::vector<std::string>());
            for (const auto& actionName : actions)
            {
                actionTypes.emplace_back(config.getActionID(actionName));
            }

            config.actionCategories[category] = actionTypes;
        }		

        //Parse Entity Categories
        nodes = gameDescription["Entities"];
        types = nodes.as<std::map<std::string, YAML::Node>>();

        for (auto& type : types)
        {
            // Parse entity category
            auto category = parseEntityCategory(type.first);
            
            // Assign entity types 
            std::vector<int> entityTypes;
            auto entities = type.second.as<std::vector<std::string>>(std::vector<std::string>());
            for (const auto& actionName : entities)
            {
                entityTypes.emplace_back(config.getEntityID(actionName));
            }

            config.entityCategories[category] = entityTypes;
        }


    }

    void GameConfigParser::parseGameRunner(const YAML::Node& gameRunner, GameConfig& config) const
    {
        auto agentInitializationTime = gameRunner["AgentInitializationTime"];
        config.shouldCheckInitTime= agentInitializationTime["Enabled"].as<bool>(false);
        config.initBudgetTimetMs= agentInitializationTime["BudgetTimeMs"].as<long>(40);
        config.initDisqualificationBudgetTimeMs = agentInitializationTime["DisqualificationTimeMs"].as<long>(60);

        auto agentComputationTime = gameRunner["AgentComputationTime"];
        config.shouldCheckComputationTime= agentComputationTime["Enabled"].as<bool>(false);
        config.budgetTimeMs= agentComputationTime["BudgetTimeMs"].as<long>(40);
        config.disqualificationBudgetTimeMs = agentComputationTime["DisqualificationTimeMs"].as<long>(60);
        config.maxNumberWarnings = agentComputationTime["MaxNumberWarnings"].as<int>(3);
    }

    void GameConfigParser::parseRenderConfig(const YAML::Node& configNode, GameConfig& config) const
    {
        config.renderConfig = std::make_unique<RenderConfig>();

        for (const auto& entityNode : loadNode(configNode, "Entities", config))
        {
            auto entityName = entityNode.first.as<std::string>();
            auto entityConfig = entityNode.second;
            config.renderConfig->entitySpritePaths.emplace(entityName, parseFilePath(entityConfig["Sprite"], config));
        }       

        //Read GameRenderer configuration
        const auto gameRendererNode = loadNode(configNode, "GameRenderer", config);
        if (gameRendererNode.IsDefined())
        {
            //Read resolution
            const auto resolutionNode = gameRendererNode["Resolution"];
            if (resolutionNode.IsDefined())
            {
                int width = resolutionNode["Width"].as<int>(800);
                int height = resolutionNode["Height"].as<int>(600);
                config.renderConfig->resolution = { width, height };
            }
            else
            {
                config.renderConfig->resolution = { 800, 600 };
            }

            //Read Default Assets
            const auto defaultAssetsNode = gameRendererNode["Default Assets"];
            if (defaultAssetsNode.IsDefined())
            {
                const auto fogOfWarNode = defaultAssetsNode["FogOfWar"];
                if (fogOfWarNode.IsDefined())
                {
                    std::string fogPath = parseFilePath(fogOfWarNode,config);
                   config.renderConfig->tileSpritePaths.emplace("FogOfWar", fogPath);
                }
                else
                {
                    throw std::runtime_error("Fog Of War not defined in GameRenderer section");

                }
                
                const auto selectedNode = defaultAssetsNode["Selected"];
                if (selectedNode.IsDefined())
                {
                    
                    std::string selectedPath = parseFilePath(selectedNode, config);
                    config.renderConfig->selectedPath = selectedPath;
                }
                else
                {
                    throw std::runtime_error("Selected not defined in GameRenderer section");
                }
            }
            else
            {
                throw std::runtime_error("Default Assets not defined in GameRenderer section");                
            }

            //Load shader
            const auto shaderNode = gameRendererNode["OutlineShader"];
            if (shaderNode.IsDefined())
            {
                config.renderConfig->outlineShaderPath= parseFilePath(shaderNode, config);
            }
            else
            {
                if (config.resourcesPath != "")
                {
                    ghc::filesystem::path path(config.resourcesPath);
                    path = path / "assets/OutLine.frag";
                    config.renderConfig->outlineShaderPath = path.string();
                }
                else
                {
                    using namespace ghc::filesystem;

                    path filePath = "../../assets/OutLine.frag";
                    // Convert path to an absolute path relative to the path of the configuration file
                    auto tmp = current_path();
                    current_path(canonical(path(config.yamlPath).parent_path()));
                    filePath = canonical(filePath);
                    current_path(tmp);
                    config.renderConfig->outlineShaderPath = filePath.string();
                }
            }

            //Load Font
            const auto fontNode = gameRendererNode["Font"];
            if (fontNode.IsDefined())
            {
                config.renderConfig->fontPath= parseFilePath(fontNode, config);
            }
            else
            {
                if (config.resourcesPath != "")
                {
                    ghc::filesystem::path path(config.resourcesPath);
                    path = path / "assets/arial.ttf";
                    config.renderConfig->fontPath = path.string();
                }
                else
                {
                    using namespace ghc::filesystem;

                    path filePath = "../../assets/arial.ttf";
                    // Convert path to an absolute path relative to the path of the configuration file
                    auto tmp = current_path();
                    current_path(canonical(path(config.yamlPath).parent_path()));
                    filePath = canonical(filePath);
                    current_path(tmp);
                    config.renderConfig->fontPath = filePath.string();
                }
            }
            
            //Load EntityCircleCollider only on RTS
            if (config.gameType == SGA::GameType::RTS)
            {
                const auto entityColliderNode = gameRendererNode["EntityCollider"];
                if (entityColliderNode.IsDefined())
                {
                    config.renderConfig->entityCircleColliderPath = parseFilePath(entityColliderNode, config);
                }
                else
                {
                    if (config.resourcesPath != "")
                    {
                        ghc::filesystem::path path(config.resourcesPath);
                        path = path / "assets/Tiles/circleCollider.png";
                        config.renderConfig->entityCircleColliderPath = path.string();
                    }
                    else
                    {
                        using namespace ghc::filesystem;

                        path filePath = "../../assets/Tiles/circleCollider.png";
                        // Convert path to an absolute path relative to the path of the configuration file
                        auto tmp = current_path();
                        current_path(canonical(path(config.yamlPath).parent_path()));
                        filePath = canonical(filePath);
                        current_path(tmp);
                        config.renderConfig->entityCircleColliderPath = filePath.string();
                    }
                }
            }
            
        }
        else
        {
            throw std::runtime_error("GameRender not defined in yaml");
        }       

        for (const auto& tileNode : loadNode(configNode, "Tiles", config))
        {
            auto tileName = tileNode.first.as<std::string>();
            auto tileConfig = tileNode.second;
            config.renderConfig->tileSpritePaths.emplace(tileName, parseFilePath(tileConfig["Sprite"], config));
        }
    }

    void GameConfigParser::parsePlayers(const YAML::Node& playerNode, GameConfig& config) const
	{

		//Parse parameters
        auto parametersNode = playerNode["Parameters"];
        parseParameterList(parametersNode, config, config.playerParameterTypes);

       config.playerSpawnableTypes = parseEntityGroup(playerNode["CanSpawn"], config);
	}

    void GameConfigParser::parseParameterList(const YAML::Node& parameterNode, GameConfig& config, std::unordered_map<ParameterID, Parameter>& parameterBucket) const
	{
        for (const auto& nameParamPair : parameterNode.as<std::map<std::string, YAML::Node>>(std::map<std::string, YAML::Node>()))
        {
            // Assign IDs to parameters that do not exist yet
            if (config.parameters.find(nameParamPair.first) == config.parameters.end())
            {
                config.parameters.insert({ nameParamPair.first, static_cast<int>(config.parameters.size()) });
            }

            //Check if is a vector of min, max and default
            if (nameParamPair.second.IsSequence())
            {
                auto parameter = nameParamPair.second.as<std::vector<double>>();
                if (parameter.size() == 3)
                {
                    // Construct the parameter
                    Parameter param;
                    param.setID(config.parameters.at(nameParamPair.first));
                    param.setName(nameParamPair.first);
                    param.setMinValue(parameter[0]);
                    param.setDefaultValue(parameter[1]);
                    param.setMaxValue(parameter[2]);               
                    param.setIndex(static_cast<int>(parameterBucket.size()));
                    parameterBucket.insert({ param.getID(), std::move(param) });
                }
                else
                    throw std::runtime_error("Parameter definition does not follow the template: [min, default value, max]");
            }
            else
            {
                Parameter param;
                param.setID(config.parameters.at(nameParamPair.first));
                param.setName(nameParamPair.first);
                param.setMinValue(0);
                param.setMaxValue(nameParamPair.second.as<double>());
                param.setDefaultValue(param.getMaxValue());
                param.setIndex(static_cast<int>(parameterBucket.size()));
                parameterBucket.insert({ param.getID(), std::move(param) });
            }
        }
	}

    std::unordered_set<EntityTypeID> GameConfigParser::parseEntityGroup(const YAML::Node& groupNode, const GameConfig& config) const
	{
        if(!groupNode.IsDefined())
        {
            return config.entityGroups.at("All");
        }
		
		if(groupNode.IsScalar())
		{
            auto groupName = groupNode.as<std::string>();
            auto groupIt = config.entityGroups.find(groupName);
			if(groupIt == config.entityGroups.end())
			{
                throw std::runtime_error("There exists no entity-group with the name " + groupName);
			}
        
            return groupIt->second;
		}
		
        if(groupNode.IsSequence())
        {
            std::unordered_set<EntityTypeID> anonymousGroup;
            auto names = groupNode.as<std::vector<std::string>>();
        	for(const auto& name : names)
        	{
                auto groupIt = config.entityGroups.find(name);
                if (groupIt == config.entityGroups.end())
                {
                    throw std::runtime_error("There exists no entity-group with the name " + name);
                }
        
                std::copy(groupIt->second.begin(), groupIt->second.end(), std::inserter(anonymousGroup, anonymousGroup.end()));
        	}
        
            return anonymousGroup;
        }

        throw std::runtime_error("Encountered an unknown Node-Type when parsing a entity-group");
	}

    std::unordered_map<ParameterID, double> GameConfigParser::parseCost(const YAML::Node& costNode, const GameConfig& config) const
	{
        auto nameCostMap = costNode.as<std::map<std::string, double>>(std::map<std::string, double>());
        std::unordered_map<ParameterID, double> idCostMap;

		for(const auto& nameCostPair : nameCostMap)
		{
            auto it = config.parameters.find(nameCostPair.first);
			if(it == config.parameters.end())
			{
                throw std::runtime_error("Could not find a parameter with the name " + nameCostPair.first);
			}

            idCostMap.emplace(it->second, nameCostPair.second);
		}

        return idCostMap;
	}

    std::string GameConfigParser::parseFilePath(const YAML::Node& pathNode, const GameConfig& config) const
    {
        if (!pathNode.IsScalar())
            throw std::runtime_error("Received a invalid file-path");

        using namespace ghc::filesystem;

        path filePath = pathNode.as<std::string>();
        // Convert path to an absolute path relative to the path of the configuration file
        auto tmp = current_path();
        current_path(canonical(path(config.yamlPath).parent_path()));
        filePath = canonical(filePath);
        current_path(tmp);
        return filePath.string();       
    }
	
	void GameConfigParser::parseMaps(const YAML::Node& mapsLayouts, std::unordered_map<int, LevelDefinition>& levelDefinitions, const GameConfig& config) const
	{
        //Read the multiple maps in this file
        auto mapsLayout = mapsLayouts["Maps"].as<std::map<std::string, YAML::Node>>();

        //Read maps
        for (auto& map : mapsLayout)
        {
            parseLevelDefinition(map.second, map.first, levelDefinitions, config);
        }
	}

    void GameConfigParser::parseLevelDefinition(const YAML::Node& mapLayout, std::string mapName,
        std::unordered_map<int, LevelDefinition>& levelDefinitions, const GameConfig& config) const
    {
        std::string mapString = mapLayout.as<std::string>();
    	
        // Remove whitespaces but keep newLines
        mapString.erase(std::remove_if(mapString.begin(), mapString.end(),
            [](char x) { return x != '\n' && std::isspace(x); }), mapString.end());    	
        
    	//Types
        std::vector<std::shared_ptr<TileType>> tileTypes;
        std::vector<EntityPlacement> entityPlacements;
    	
    	// Create some lookups for initializing the board and entities
        std::unordered_map<char, const TileType*> tileLookup;
        const auto* defaultTile = &config.tileTypes.begin()->second;
        for (const auto& idTilePair : config.tileTypes)
        {
            tileLookup.emplace(idTilePair.second.getSymbol(), &idTilePair.second);
            if (idTilePair.second.isDefaultTile())
                defaultTile = &idTilePair.second;
        }

        std::unordered_map<char, const EntityType*> entityLookup;
        for (const auto& idEntityPair : config.entityTypes)
        {
            entityLookup.emplace(idEntityPair.second.getSymbol(), &idEntityPair.second);
        }
    	
        // Configure new level definition and entity placements
        auto x = 0;
        auto y = 0;
        auto width = -1;

        for (size_t i = 0; i < mapString.size(); i++)
        {
            auto c = mapString[i];
            if (c == '\n')
            {
                y++;
                if (width == -1)
                {
                    width = x;
                }
                else if (x != width)
                {
                    throw std::runtime_error("Line " + std::to_string(y) + " contains " + std::to_string(x) + " symbols. Expected " + std::to_string(width));
                }

                x = 0;
                continue;
            }

            auto entityIt = entityLookup.find(c);
            auto tileIt = tileLookup.find(c);
            if (entityIt != entityLookup.end())
            {
                // Check if the entity was assigned to an player, we only look for players with ID 0-9
                auto ownerID = Player::getNeutralPlayerID();
                if (i < mapString.size() - 1 && std::isdigit(mapString[i + 1]))
                {
                    ownerID = (mapString[i + 1] - '0'); // Convert char '0','1',... to the corresponding integer
                    if (ownerID>= static_cast<int>(config.getNumberOfPlayers()))
                    {
                        throw std::runtime_error("Tried assigning the entity " + entityIt->second->getName() + " to an unknown player " + std::to_string(ownerID));
                    }
                    i++;
                }

                EntityPlacement newEntity;
                newEntity.position = Vector2f(x, y);
                newEntity.ownerID = ownerID;
                newEntity.entityType = std::make_shared<EntityType>(*entityIt->second);
                entityPlacements.emplace_back(newEntity);
            	
                // Since an entity occupied this position, we will place the default tile here
                tileTypes.emplace_back(std::make_shared<TileType>(*defaultTile));
            }
            else if (tileIt != tileLookup.end())
            {                
                tileTypes.emplace_back(std::make_shared<TileType>(*tileIt->second));
            }
            else
            {
                throw std::runtime_error("Encountered unknown symbol '" + std::string(1, c) + "'when parsing the board.");
            }

            x++;
        }

        // Sometimes there is a newLine at the end of the string, and sometimes not
        if (mapString[mapString.size() - 1] != '\n')
        {
            y++;
            if (x != width)
            {
                throw std::runtime_error("Line " + std::to_string(y) + " contains " + std::to_string(x) + " symbols. Expected " + std::to_string(width));
            }
        }   	
       
    	
        //Assign grid and entity placements
        LevelDefinition newLevel(entityPlacements,Grid2D<std::shared_ptr<TileType>>(static_cast<size_t>(width), tileTypes.begin(), tileTypes.end()));
        newLevel.name = mapName;
        newLevel.boardString = mapString;

    	//Add new level definition
        levelDefinitions.emplace(static_cast<int>(levelDefinitions.size()), newLevel);
    }

    void  GameConfigParser::parseModifiers(const YAML::Node& modifierNode, GameConfig& config, std::unordered_map< ParameterID, double >& modifiers) const
    {
       for(const auto& nameParamPair : modifierNode.as< std::map< std::string, double > >(
              std::map< std::string, double >())) {
          // Check if parameter is found
          if(config.parameters.find(nameParamPair.first) == config.parameters.end()) {
             throw std::runtime_error("Cannot find parameter for Buffs");
          }

          modifiers.insert(
             {config.parameters.find(nameParamPair.first)->second, nameParamPair.second});
       }
    }

    void GameConfigParser::parseBuffs(const YAML::Node& buffsNode, GameConfig& config) const
    {
       if(! buffsNode.IsDefined()) {
          throw std::runtime_error("Cannot find definition for Buffs");
       }

       auto types = buffsNode.as< std::map< std::string, YAML::Node > >();
       for(const auto& nameTypePair : types) {
          BuffType type;
          type.setName(nameTypePair.first);

          type.setID(static_cast< int >(config.buffsTypes.size()));

          //Parse modifiers
          // AdditiveModifier
          std::unordered_map< ParameterID, double > additiveModifiers;
          parseModifiers(nameTypePair.second["AdditiveModifier"], config, additiveModifiers);
          type.setAdditiveModifiers(additiveModifiers);
          // MultiplicationModifiers
          std::unordered_map< ParameterID, double > multiplicationModifiers;
          parseModifiers(nameTypePair.second["MultiplicationModifier"], config, multiplicationModifiers);
          type.setMultiplicationModifiers(multiplicationModifiers);

          //Add buff type
           config.buffsTypes.emplace(type.getID(), std::move(type));
       }
    }
    }
