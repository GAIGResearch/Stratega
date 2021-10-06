
#include <Stratega/core/TestUtils/utils.hpp>
#include <Mocks/Stratega/core/MockGrid.hpp>
#include <Mocks/Stratega/core/MockGameRunner.hpp>
#include <Mocks/Stratega/core/MockGameConfig.hpp>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "Stratega/Configuration/GameConfigParser.h"
#include "Stratega/Game/TBSGameRunner.h"

using ::testing::ContainerEq;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using ::testing::Eq;
using ::testing::Mock;
using ::testing::Return;
using ::testing::UnorderedElementsAre;

using ::testing::_;
namespace SGA
{
    
    TEST(ConfigurationTest, loadAgents)
	{
        auto agentsNode = loadAndGetNode("test/resources/agents.yaml", "Agents");

       //auto mockGameConfigPtr = std::shared_ptr<MockGameConfig>(new MockGameConfig());

        GameConfig gameConfig;

        GameConfigParser parser;
        parser.parseAgents(agentsNode, gameConfig);

        auto agents = gameConfig.generateAgents();

        //Check agents
        ASSERT_EQ(agents.size(), 2);
        ASSERT_EQ(agents[0], nullptr);
        ASSERT_EQ(agents[1]->getName(), "OSLAAgent");
    }

    TEST(ConfigurationTest, loadPlayerDefinition)
    {
        auto playerNode = loadAndGetNode("test/resources/configurationTest.yaml", "Player");
        auto entityGroupsNode = loadAndGetNode("test/resources/configurationTest.yaml", "EntityGroups");
        auto entitiesNode = loadAndGetNode("test/resources/configurationTest.yaml", "Entities");
        auto actionsNode = loadAndGetNode("test/resources/configurationTest.yaml", "Actions");

        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());

        parserPtr->parseEntities(entitiesNode, *gameConfigPtr);
        parserPtr->parseEntityGroups(entityGroupsNode, *gameConfigPtr);
        parserPtr->parsePlayers(playerNode, *gameConfigPtr);

        ASSERT_EQ(gameConfigPtr->playerParameterTypes.size(), 7);
        ASSERT_EQ(gameConfigPtr->playerSpawnableTypes.size(), 2);
        ASSERT_EQ(gameConfigPtr->playerActionIds.size(), 0);

        //Load Actions
        parserPtr->parseActions(actionsNode, *gameConfigPtr);
        parserPtr->assignPlayerActions(playerNode, *gameConfigPtr);
        ASSERT_EQ(gameConfigPtr->playerActionIds.size(), 1);

        auto& playerParameters = gameConfigPtr->playerParameterTypes;
        auto* parameter1=findParameterType(playerParameters, "Food");
        auto* parameter2=findParameterType(playerParameters, "Gold");
        ASSERT_TRUE(parameter1 != nullptr);
        ASSERT_TRUE(parameter2 != nullptr);

        ASSERT_EQ(parameter1->getMaxValue(), 100);
        ASSERT_EQ(parameter1->getMinValue(), 0);
        ASSERT_EQ(parameter1->getDefaultValue(), 100);

        ASSERT_EQ(parameter2->getMaxValue(), 0);
        ASSERT_EQ(parameter2->getMinValue(), 0);
        ASSERT_EQ(parameter2->getDefaultValue(), 0);
    }

    TEST(ConfigurationTest, loadEntityGroups)
    {
        auto entityGroupsNode = loadAndGetNode("test/resources/configurationTest.yaml", "EntityGroups");
        auto entitiesNode = loadAndGetNode("test/resources/configurationTest.yaml", "Entities");

        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());

        parserPtr->parseEntities(entitiesNode, *gameConfigPtr);
        parserPtr->parseEntityGroups(entityGroupsNode, *gameConfigPtr);

        ASSERT_EQ(gameConfigPtr->entityGroups.size(), 7);
        ASSERT_EQ(gameConfigPtr->entityGroups.find("Units")->second.size(), 2);
        ASSERT_EQ(gameConfigPtr->entityGroups.find("Units2")->second.size(), 1);
    }
    
    TEST(ConfigurationTest, loadEntitiesDefinition)
    {
        auto entityGroupsNode = loadAndGetNode("test/resources/configurationTest.yaml", "EntityGroups");
        auto entitiesNode = loadAndGetNode("test/resources/configurationTest.yaml", "Entities");

        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());

        parserPtr->parseEntities(entitiesNode, *gameConfigPtr);
        parserPtr->parseEntityGroups(entityGroupsNode, *gameConfigPtr);

        ASSERT_EQ(gameConfigPtr->entityTypes.size(), 4);

        auto* entityType0 = findEntityType(gameConfigPtr->entityTypes, "Archer");
        ASSERT_TRUE(entityType0 != nullptr);
        ASSERT_EQ(entityType0->getActionIDs().size(), 0);
        ASSERT_EQ(entityType0->getLoSRange(), 10);
        ASSERT_EQ(entityType0->getParameters().size(), 4);

        
        ASSERT_EQ(entityType0->getParameter(gameConfigPtr->parameters.find("MovementPoints")->second).getName(), "MovementPoints");
        ASSERT_EQ(entityType0->getParameter(gameConfigPtr->parameters.find("MovementPoints")->second).getDefaultValue(), 2);
        ASSERT_EQ(entityType0->getSymbol(), 'a');
        

        auto actionsNode = loadAndGetNode("test/resources/configurationTest.yaml", "Actions");
        parserPtr->parseActions(actionsNode, *gameConfigPtr);
        parserPtr->assignEntitiesActions(entitiesNode, *gameConfigPtr);
        ASSERT_EQ(entityType0->getActionIDs().size(), 2);
    }


    TEST(ConfigurationTest, loadActions)
    {
        auto playerNode = loadAndGetNode("test/resources/configurationTest.yaml", "Player");
        auto entityGroupsNode = loadAndGetNode("test/resources/configurationTest.yaml", "EntityGroups");
        auto entitiesNode = loadAndGetNode("test/resources/configurationTest.yaml", "Entities");
        auto actionsNode = loadAndGetNode("test/resources/configurationTest.yaml", "Actions");

        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());

        parserPtr->parseEntities(entitiesNode, *gameConfigPtr);
        parserPtr->parseEntityGroups(entityGroupsNode, *gameConfigPtr);

        //Load Actions
        parserPtr->parseActions(actionsNode, *gameConfigPtr);
        ASSERT_EQ(gameConfigPtr->actionTypes.size(), 4);

        //Check actions
        auto& actionTypes = gameConfigPtr->actionTypes;
        auto* action0 = findActionType(actionTypes, "AttackPlayer");
        ASSERT_TRUE(action0!=nullptr);
        ASSERT_EQ(action0->getSourceType(), ActionSourceType::Player);
        ASSERT_EQ(action0->getEffects().size(), 1);
        ASSERT_EQ(action0->getEffects()[0]->expr(), "Attack(Target.Health, Source.AttackDamage)");
        ASSERT_EQ(action0->getCooldown(), 1);

        auto& targets = action0->getTargets();
        ASSERT_EQ(targets.size(),1);
        ASSERT_EQ(targets[0].second.size(),1);
        ASSERT_EQ(targets[0].second[0]->expr(),"DifferentPlayer(Source, Target)");

        ASSERT_EQ(targets[0].first.getType(), TargetType::Entity);

        ASSERT_EQ(targets[0].first.getSamplingMethod()->name, "Neighbours");

        ASSERT_EQ(targets[0].first.getTypeString(), "Entity");
    }


    TEST(ConfigurationTest, loadForwardModelDefinition)
    {
        auto fmNode = loadAndGetNode("test/resources/configurationTest.yaml", "ForwardModel");

        auto playerNode = loadAndGetNode("test/resources/configurationTest.yaml", "Player");
        auto entityGroupsNode = loadAndGetNode("test/resources/configurationTest.yaml", "EntityGroups");
        auto entitiesNode = loadAndGetNode("test/resources/configurationTest.yaml", "Entities");
        auto actionsNode = loadAndGetNode("test/resources/configurationTest.yaml", "Actions");

        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());
        parserPtr->parseEntities(entitiesNode, *gameConfigPtr);
        parserPtr->parseEntityGroups(entityGroupsNode, *gameConfigPtr);

        parserPtr->parseForwardModel(fmNode, *gameConfigPtr);

        ASSERT_EQ(gameConfigPtr->gameType, GameType::TBS);
        ASSERT_EQ(gameConfigPtr->forwardModel->getWinConditions().size(), 1);
        ASSERT_EQ(gameConfigPtr->forwardModel->getLoseConditions().size(), 1);

        ASSERT_EQ(gameConfigPtr->forwardModel->getLoseConditions()[0][0]->expr(), "HasNoEntity(Source, King)");


        ASSERT_EQ(gameConfigPtr->forwardModel->getOnEntitySpawnEffects().size(), 0);
        ASSERT_EQ(gameConfigPtr->forwardModel->getOnTickEffects().size(), 1);
        ASSERT_EQ(gameConfigPtr->forwardModel->getOnTickEffects()[0].validTargets.size(), 4);
        ASSERT_EQ(gameConfigPtr->forwardModel->getOnTickEffects()[0].conditions.size(), 0);
        ASSERT_EQ(gameConfigPtr->forwardModel->getOnTickEffects()[0].effects.size(), 1);
        ASSERT_EQ(gameConfigPtr->forwardModel->getOnTickEffects()[0].effects[0]->expr(), "SetToMaximum(Source.MovementPoints)");
    }

    TEST(ConfigurationTest, loadGameDescriptionDefinition)
    {
        auto fmNode = loadAndGetNode("test/resources/configurationTest.yaml", "ForwardModel");
        auto gdNode = loadAndGetNode("test/resources/configurationTest.yaml", "GameDescription");

        auto entityGroupsNode = loadAndGetNode("test/resources/configurationTest.yaml", "EntityGroups");
        auto entitiesNode = loadAndGetNode("test/resources/configurationTest.yaml", "Entities");
        auto actionsNode = loadAndGetNode("test/resources/configurationTest.yaml", "Actions");

        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());
        parserPtr->parseEntities(entitiesNode, *gameConfigPtr);        
        parserPtr->parseEntityGroups(entityGroupsNode, *gameConfigPtr);
        parserPtr->parseActions(actionsNode, *gameConfigPtr);

        parserPtr->parseForwardModel(fmNode, *gameConfigPtr);

        parserPtr->parseGameDescription(gdNode, *gameConfigPtr);

        ASSERT_EQ(gameConfigPtr->actionCategories.size(), 3);
        ASSERT_EQ(gameConfigPtr->entityCategories.size(), 5);

        ASSERT_EQ(gameConfigPtr->entityCategories[EntityCategory::Unit].size(), 4);
        ASSERT_EQ(gameConfigPtr->actionCategories[ActionCategory::Attack].size(), 2);
    }

    TEST(ConfigurationTest, loadGameRunnerDefinition)
    {
        auto grNode = loadAndGetNode("test/resources/configurationTest.yaml", "GameRunner");

        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());

        parserPtr->parseGameRunner(grNode, *gameConfigPtr);

        ASSERT_EQ(gameConfigPtr->shouldCheckInitTime, false);
        ASSERT_EQ(gameConfigPtr->initBudgetTimetMs, 50);
        ASSERT_EQ(gameConfigPtr->initDisqualificationBudgetTimeMs, 70);

        ASSERT_EQ(gameConfigPtr->shouldCheckComputationTime, true);
        ASSERT_EQ(gameConfigPtr->budgetTimeMs, 1000);
        ASSERT_EQ(gameConfigPtr->disqualificationBudgetTimeMs, 70);
        ASSERT_EQ(gameConfigPtr->maxNumberWarnings, 5);

    }

    TEST(ConfigurationTest, loadGameRunnerDefinitionByDefault)
    {
        auto grNode = loadAndGetNode("test/resources/configurationTest.yaml", "GameRunner");

        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());

        ASSERT_EQ(gameConfigPtr->shouldCheckInitTime, false);
        ASSERT_EQ(gameConfigPtr->initBudgetTimetMs, 40);
        ASSERT_EQ(gameConfigPtr->initDisqualificationBudgetTimeMs, 60);

        ASSERT_EQ(gameConfigPtr->shouldCheckComputationTime, false);
        ASSERT_EQ(gameConfigPtr->budgetTimeMs, 40);
        ASSERT_EQ(gameConfigPtr->disqualificationBudgetTimeMs, 60);
        ASSERT_EQ(gameConfigPtr->maxNumberWarnings, 3);
    }
}