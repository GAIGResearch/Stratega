#include <filesystem>
#include <Stratega/Configuration/GameConfigParser.h>
#include <Stratega/Agent/AgentFactory.h>
#include <yaml-cpp/yaml.h>

namespace SGA
{
    std::unique_ptr<GameConfig> loadConfigFromYAML(const std::string& filePath)
    {
        GameConfigParser parser;
        return parser.parseFromFile(filePath);
    }

	
	std::unique_ptr<GameConfig> GameConfigParser::parseFromFile(const std::string& filePath) const
	{
		auto configNode = YAML::LoadFile(filePath);
        auto config = std::make_unique<GameConfig>();
        config->yamlPath = filePath;
        config->gameType = configNode["GameConfig"]["Type"].as<GameType>();
        config->tickLimit = configNode["GameConfig"]["RoundLimit"].as<int>(config->tickLimit);
        config->numPlayers = configNode["GameConfig"]["PlayerCount"].as<int>(config->numPlayers);

		// Parse complex structures
		// Order is important, only change if you are sure that a function doesn't depend on something parsed before it
		parseEntities(configNode["Entities"], *config);
        parseEntityGroups(configNode["EntityGroups"], *config);
        parseAgents(configNode["Agents"], *config);
        parseTileTypes(configNode["Tiles"], *config);
        parseBoardGenerator(configNode["Board"], *config);
        parsePlayers(configNode["Player"], *config);

		if(configNode["TechnologyTrees"].IsDefined())
			parseTechnologyTrees(configNode["TechnologyTrees"], *config);
		
        parseActions(configNode["Actions"], *config);
        parseForwardModel(configNode["ForwardModel"], *config);

        if (configNode["GameDescription"].IsDefined())
			parseActionCategories(configNode["GameDescription"], *config);

		//Assign actions to entities
        // Parse additional configurations for entities that couldn't be handled previously
        auto types = configNode["Entities"].as<std::map<std::string, YAML::Node>>();
        for (auto& type : config->entityTypes)
        {
            // Assign actions to entities
            auto actions = types[type.second.name]["Actions"].as<std::vector<std::string>>(std::vector<std::string>());
            for (const auto& actionName : actions)
            {
                type.second.actionIds.emplace_back(config->getActionID(actionName));
            }

            // Data for hardcoded condition canSpawn => Technology-requirements and spawnable-entities
            type.second.spawnableEntityTypes = parseEntityGroup(types[type.second.name]["CanSpawn"], *config);
            auto name = types[type.second.name]["RequiredTechnology"].as<std::string>("");
            type.second.requiredTechnologyID = name.empty() ? TechnologyTreeType::UNDEFINED_TECHNOLOGY_ID : config->technologyTreeCollection.getTechnologyTypeID(name);
        	// Hardcoded cost information
            type.second.cost = parseCost(types[type.second.name]["Cost"], *config);
        }
		
		//Assign player actions
        auto actions = configNode["Player"]["Actions"].as<std::vector<std::string>>(std::vector<std::string>());
        for (const auto& actionName : actions)
        {
            config->playerActionIds.emplace_back(config->getActionID(actionName));
        }

    	// Parse render data - ToDo split into the dedicated functions (Entity, Tile, etc)
        parseRenderConfig(configNode, *config);
		    		
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
        fogOfWarType.id = idCounter++;
        fogOfWarType.name = "FogOfWar";
        fogOfWarType.isWalkable = false;
        fogOfWarType.symbol = '_';
        config.tileTypes.emplace(fogOfWarType.id, std::move(fogOfWarType));

		for(const auto& nameConfigPair : tileConfigs)
		{
            TileType type;
            type.id = idCounter++;
            type.name = nameConfigPair.first;
			type.isWalkable = nameConfigPair.second["IsWalkable"].as<bool>(type.isWalkable);
			type.blocksSight = nameConfigPair.second["BlocksSight"].as<bool>(type.blocksSight);
            type.isDefaultTile = nameConfigPair.second["DefaultTile"].as<bool>(false);
			type.symbol = nameConfigPair.second["Symbol"].as<char>();
            config.tileTypes.emplace(type.id, std::move(type));
		}
	}

    void GameConfigParser::parseBoardGenerator(const YAML::Node& boardNode, GameConfig& config) const
	{
		if(!boardNode.IsDefined())
		{
            throw std::runtime_error("Cannot find definition for the Board");
		}

        if (boardNode["GenerationType"].as<std::string>() == "Manual")
        {
            config.boardString = boardNode["Layout"].as<std::string>();
        	// Remove whitespaces but keep newLines
            config.boardString.erase(std::remove_if(config.boardString.begin(), config.boardString.end(), 
                [](char x) { return x != '\n' && std::isspace(x); }), config.boardString.end());
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
            type.name = nameTypePair.first;
            type.symbol = nameTypePair.second["Symbol"].as<char>('\0');
            type.id = static_cast<int>(config.entityTypes.size());
            type.lineOfSight = nameTypePair.second["LineOfSightRange"].as<double>();

            parseParameterList(nameTypePair.second["Parameters"], config, type.parameters);

            type.continuousActionTime = nameTypePair.second["Time"].as<int>(0);
            config.entityTypes.emplace(type.id, std::move(type));
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
            config.entityGroups.emplace(idEntityPair.second.name, std::initializer_list<int>{ idEntityPair.first });
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
            type.id = static_cast<int>(config.actionTypes.size());
            type.name = nameTypePair.first;
        	
            context.targetIDs.emplace("Source", 0);
        	
        	//Parse all the targets
            for (auto& target : nameTypePair.second["Targets"].as<std::map<std::string, YAML::Node>>())
            {
                TargetType newTarget;
                context.targetIDs.emplace(target.first, context.targetIDs.size());
                newTarget = parseTargetType(target.second, config);
            	
                std::vector<std::shared_ptr<Condition>> targetConditionsList;            	
                //// Parse target conditions
		        auto targetConditions = target.second["Conditions"].as<std::vector<std::string>>(std::vector<std::string>());
		        parser.parseFunctions(targetConditions, targetConditionsList, context);            	
                type.actionTargets.emplace_back(newTarget, targetConditionsList);
            }
        	
            type.sourceType = nameTypePair.second["Type"].as<ActionSourceType>();
            type.cooldownTicks = nameTypePair.second["Cooldown"].as<int>(0);
        	
            // Parse preconditions            
        	auto preconditions = nameTypePair.second["Preconditions"].as<std::vector<std::string>>(std::vector<std::string>());
            parser.parseFunctions(preconditions, type.preconditions, context);

            // Parse effects
            auto effects = nameTypePair.second["Effects"].as<std::vector<std::string>>(std::vector<std::string>());
            parser.parseFunctions(effects, type.effects, context);

            type.isContinuous = false;
        	
        	//Continuous Action Stuff
            if (nameTypePair.second["TriggerComplete"].IsDefined())
            {
                type.isContinuous = true;
                auto targetConditions = nameTypePair.second["TriggerComplete"].as<std::vector<std::string>>(std::vector<std::string>());
                parser.parseFunctions(targetConditions, type.triggerComplete, context);
            }
        	
            if (nameTypePair.second["OnStart"].IsDefined())
            {
                type.isContinuous = true;

                auto effectStrings = nameTypePair.second["OnStart"].as<std::vector<std::string>>(std::vector<std::string>());
                parser.parseFunctions(effectStrings, type.OnStart, context);
            }

            if (nameTypePair.second["OnTick"].IsDefined())
            {
                type.isContinuous = true;

                auto effectStrings = nameTypePair.second["OnTick"].as<std::vector<std::string>>(std::vector<std::string>());
                parser.parseFunctions(effectStrings, type.OnTick, context);
            }

            if (nameTypePair.second["OnComplete"].IsDefined())
            {
                type.isContinuous = true;

                auto effectStrings = nameTypePair.second["OnComplete"].as<std::vector<std::string>>(std::vector<std::string>());
                parser.parseFunctions(effectStrings, type.OnComplete, context);
            }

            if (nameTypePair.second["OnAbort"].IsDefined())
            {
                type.isContinuous = true;

                auto effectStrings = nameTypePair.second["OnAbort"].as<std::vector<std::string>>(std::vector<std::string>());
                parser.parseFunctions(effectStrings, type.OnAbort, context);
            }
        	
            config.actionTypes.emplace(type.id, std::move(type));
            context.targetIDs.clear();
        }
    }

    TargetType GameConfigParser::parseTargetType(const YAML::Node& node, const GameConfig& config) const
    {
        TargetType targetType;
        targetType.type = node["Type"].as<TargetType::Type>();
        if (targetType.type == TargetType::Position)
        {
        	if(node["Shape"].IsDefined())
        	{

                targetType.shapeType = node["Shape"].as<ShapeType>();
                targetType.shapeSize = node["Size"].as<int>(0);
        	}
        }
        else if (targetType == TargetType::Entity || targetType == TargetType::EntityType)
        {
            targetType.groupEntityTypes = parseEntityGroup(node["ValidTargets"], config);
        }
        else if (targetType.type == TargetType::Technology)
        {
            auto techNode = node["ValidTargets"];
        	if(techNode.IsScalar() && techNode.as<std::string>() == "All")
        	{
        		for(const auto& tree: config.technologyTreeCollection.technologyTreeTypes)
        		{
        			for(const auto& tech : tree.second.technologies)
						targetType.technologyTypes.insert(tech.first);
        		}
        	}
            else if(techNode.IsSequence())
            {
                auto technologies = techNode.as<std::vector<std::string>>(std::vector<std::string>());
                //Assigne technology IDs to the technologytypes map
                for (auto& technology : technologies)
                {
                    targetType.technologyTypes.insert(config.technologyTreeCollection.getTechnologyTypeID(technology));
                }
            }
        }
        return targetType;
    }

    ActionCategory GameConfigParser::parseActionCategory(const std::string& name) const
    {
        ActionCategory actionCategory;
              
        if (name == "Attack")
            actionCategory = SGA::ActionCategory::Attack;
        else if (name == "Heal")
            actionCategory = SGA::ActionCategory::Heal;
        else if (name == "Research")
            actionCategory = SGA::ActionCategory::Research;
        else if (name == "Gather")
            actionCategory = SGA::ActionCategory::Gather;
        else if (name == "Move")
            actionCategory = SGA::ActionCategory::Move;
        else if (name == "Spawn")
            actionCategory = SGA::ActionCategory::Spawn;
        else
            throw std::runtime_error("Cannot find action category");
		
        return actionCategory;
    }

	void GameConfigParser::parseForwardModel(const YAML::Node& fmNode, GameConfig& config) const
	{
        if (!fmNode.IsDefined())
        {
            throw std::runtime_error("Cannot find a definition for the ForwardModel");
        }
		
        std::unique_ptr<EntityForwardModel> fm;
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
            fm->winConditions.emplace_back(conditionList);
        }

        //Parse Lose Conditions
        auto loseConditions = fmNode["LoseConditions"].as<std::map<std::string, YAML::Node>>(std::map<std::string, YAML::Node>());

        for (auto& loseCondition : loseConditions)
        {
            std::vector<std::shared_ptr<Condition>> conditionList;

            auto conditions = loseCondition.second.as<std::vector<std::string>>(std::vector<std::string>());
            parser.parseFunctions<Condition>(conditions, conditionList, context);
            fm->loseConditions.emplace_back(conditionList);
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
                onTickEffect.validTargets = parseEntityGroup(nameEffectsPair.second["ValidTargets"], config);
                parser.parseFunctions<Condition>(conditions, onTickEffect.conditions, context);
                parser.parseFunctions<Effect>(effects, onTickEffect.effects, context);
				// Add it to the fm
                fm->onTickEffects.emplace_back(std::move(onTickEffect));
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
                fm->onEntitySpawnEffects.emplace_back(std::move(onSpawnEffect));
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
        
		//Initialize researched list for each player
        for (int i = 0; i < static_cast<int>(config.agentParams.size()); i++)
        {
            config.technologyTreeCollection.researchedTechnologies[i] = {};
        }
	}

    void GameConfigParser::parseActionCategories(const YAML::Node& gameDescription, GameConfig& config) const
    {
        //Parse Actions
        auto actionsNode = gameDescription["Actions"];
        auto types = actionsNode.as<std::map<std::string, YAML::Node>>();

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
    }

    void GameConfigParser::parseRenderConfig(const YAML::Node& configNode, GameConfig& config) const
    {
        config.renderConfig = std::make_unique<RenderConfig>();

        // Hardcode shader Path
        config.renderConfig->outlineShaderPath = "./GUI/Assets/OutLine.frag";

        for (const auto& entityNode : configNode["Entities"])
        {
            auto entityName = entityNode.first.as<std::string>();
            auto entityConfig = entityNode.second;
            config.renderConfig->entitySpritePaths.emplace(entityName, parseFilePath(entityConfig["Sprite"], config));
        }

        //Add Fog of War tile
        config.renderConfig->tileSpritePaths.emplace("FogOfWar", "./GUI/Assets/Tiles/notVisible.png");

        for (const auto& tileNode : configNode["Tiles"])
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
        for (const auto& nameParamPair : parameterNode.as<std::map<std::string, double>>(std::map<std::string, double>()))
        {
            // Assign IDs to parameters that do not exist yet
            if (config.parameters.find(nameParamPair.first) == config.parameters.end())
            {
                config.parameters.insert({ nameParamPair.first, config.parameters.size() });
            }

            // Construct the parameter
            Parameter param;
            param.id = config.parameters.at(nameParamPair.first);
            param.name = nameParamPair.first;
            param.minValue = 0;
            param.maxValue = nameParamPair.second;
            param.defaultValue = param.maxValue;
            param.index = static_cast<int>(parameterBucket.size());
            parameterBucket.insert({ param.id, std::move(param) });
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
    	
        using namespace std::filesystem;

        path filePath = pathNode.as<std::string>();
    	// Convert path to an absolute path relative to the path of the configuration file
        auto tmp = current_path();
        current_path(canonical(path(config.yamlPath).parent_path()));
        filePath = canonical(filePath);
        current_path(tmp);

        return filePath.string();
    }
}
