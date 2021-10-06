#include <Stratega/core/TestUtils/utils.hpp>
#include <Mocks/Stratega/core/MockGameState.hpp>
#include <Mocks/Stratega/core/MockGameConfig.hpp>
#include <Mocks/Stratega/core/MockPlayer.hpp>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "Stratega/Configuration/GameConfigParser.h"

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
    TEST(GameStateTest, actionGenerationTest)
    {
        auto boardNode = loadAndGetNode("test/resources/minimalConfigTest.yaml", "Board");
        auto tilesNode = loadAndGetNode("test/resources/minimalConfigTest.yaml", "Tiles");
        auto agentsNode = loadAndGetNode("test/resources/minimalConfigTest.yaml", "Agents");
        auto entitiesNode = loadAndGetNode("test/resources/minimalConfigTest.yaml", "Entities");
        auto actionssNode = loadAndGetNode("test/resources/minimalConfigTest.yaml", "Actions");


        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());
        auto fmPtr = std::shared_ptr<TBSForwardModel>(new TBSForwardModel());

        ////Load tiles and board
        parserPtr->parseAgents(agentsNode, *gameConfigPtr);        
        parserPtr->parseTileTypes(tilesNode, *gameConfigPtr);
        parserPtr->parseEntities(entitiesNode, *gameConfigPtr);
        parserPtr->parseEntityGroups(entitiesNode["test"], *gameConfigPtr);
        
        parserPtr->parseActions(actionssNode, *gameConfigPtr);        
        parserPtr->assignEntitiesActions(entitiesNode, *gameConfigPtr);

        parserPtr->parseBoardGenerator(boardNode, *gameConfigPtr);

        auto gameState = gameConfigPtr->generateGameState();
        
        auto possibleActions=fmPtr->generateActions(*gameState, 0);

        //ASSERT_EQ(possibleActions.size(), 21);

        auto action0 = possibleActions[0];

        ASSERT_EQ(action0.getOwnerID(), 0);
        ASSERT_EQ(action0.getSourceID(), 1);
        ASSERT_EQ(action0.getActionName(), "Attack");
        ASSERT_EQ(action0.getActionType().getName(), "Attack");
        ASSERT_EQ(action0.getTargets().size(), 2);

        ASSERT_EQ(action0.getTargets()[1].getEntityID(), 0);
    }

    TEST(GameStateTest, actionExecutionTest)
    {
        auto boardNode = loadAndGetNode("test/resources/minimalConfigTest.yaml", "Board");
        auto tilesNode = loadAndGetNode("test/resources/minimalConfigTest.yaml", "Tiles");
        auto agentsNode = loadAndGetNode("test/resources/minimalConfigTest.yaml", "Agents");
        auto entitiesNode = loadAndGetNode("test/resources/minimalConfigTest.yaml", "Entities");
        auto actionssNode = loadAndGetNode("test/resources/minimalConfigTest.yaml", "Actions");


        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());
        auto fmPtr = std::shared_ptr<TBSForwardModel>(new TBSForwardModel());

        ////Load tiles and board
        parserPtr->parseAgents(agentsNode, *gameConfigPtr);        
        parserPtr->parseTileTypes(tilesNode, *gameConfigPtr);
        parserPtr->parseEntities(entitiesNode, *gameConfigPtr);
        parserPtr->parseEntityGroups(entitiesNode["test"], *gameConfigPtr);
        
        parserPtr->parseActions(actionssNode, *gameConfigPtr);        
        parserPtr->assignEntitiesActions(entitiesNode, *gameConfigPtr);

        parserPtr->parseBoardGenerator(boardNode, *gameConfigPtr);

        auto gameState = gameConfigPtr->generateGameState();
        
        auto possibleActions=fmPtr->generateActions(*gameState, 0);

        //ASSERT_EQ(possibleActions.size(), 21);

        auto action0 = possibleActions[0];

        ASSERT_EQ(action0.getTargets()[1].getEntityID(), 0);
    }
}