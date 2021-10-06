#include <Stratega/core/TestUtils/utils.hpp>
#include <Mocks/Stratega/core/MockGameState.hpp>
#include <Mocks/Stratega/core/MockGameConfig.hpp>
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
    
    TEST(BoardParseTest, testLoadBoard)
	{
        auto boardNode = loadAndGetNode("test/resources/boards/boardDefinition.yaml", "Board");
        auto tilesNode = loadAndGetNode("test/resources/boards/boardDefinition.yaml", "Tiles");

        
        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());

        ////Load tiles and board
        parserPtr->parseTileTypes(tilesNode, *gameConfigPtr);
        parserPtr->parseBoardGenerator(boardNode, *gameConfigPtr);       

        auto state=gameConfigPtr->generateGameState();

        ASSERT_EQ(gameConfigPtr->tileTypes.size(), 5);

        auto selectedLevelDefinition = gameConfigPtr->levelDefinitions.find(0);
        auto mockGameStatePtr = std::shared_ptr<MockGameState>(new MockGameState());
        auto tiles = gameConfigPtr->instanceTiles(*mockGameStatePtr, selectedLevelDefinition->second.board);

        ASSERT_EQ(tiles.size(), 130);
    }

    TEST(BoardParseTest, testLoadBoardWithoutSpaces)
    {
        auto boardNode = loadAndGetNode("test/resources/boards/boardDefinitionNoSpace.yaml", "Board");
        auto tilesNode = loadAndGetNode("test/resources/boards/boardDefinitionNoSpace.yaml", "Tiles");


        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());

        ////Load tiles and board
        parserPtr->parseTileTypes(tilesNode, *gameConfigPtr);
        parserPtr->parseBoardGenerator(boardNode, *gameConfigPtr);

        auto state = gameConfigPtr->generateGameState();

        ASSERT_EQ(gameConfigPtr->tileTypes.size(), 5);

        auto selectedLevelDefinition = gameConfigPtr->levelDefinitions.find(0);
        auto mockGameStatePtr = std::shared_ptr<MockGameState>(new MockGameState());
        auto tiles = gameConfigPtr->instanceTiles(*mockGameStatePtr, selectedLevelDefinition->second.board);

        ASSERT_EQ(tiles.size(), 130);
    }


    TEST(BoardParseTest, testLoadBoardWithUndefinedEntities)
    {
        auto boardNode = loadAndGetNode("test/resources/boards/boardDefinitionPlayerEntities.yaml", "Board");
        auto tilesNode = loadAndGetNode("test/resources/boards/boardDefinitionPlayerEntities.yaml", "Tiles");
        auto agentsNode = loadAndGetNode("test/resources/boards/boardDefinitionPlayerEntities.yaml", "Agents");


        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());

        ////Load tiles and board
        parserPtr->parseAgents(agentsNode, *gameConfigPtr);
        parserPtr->parseTileTypes(tilesNode, *gameConfigPtr);
        ASSERT_THROW(parserPtr->parseBoardGenerator(boardNode, *gameConfigPtr), std::runtime_error);
    }

    TEST(BoardParseTest, testLoadBoardWithDefinedEntities)
    {
        auto boardNode = loadAndGetNode("test/resources/boards/boardDefinitionPlayerEntities.yaml", "Board");
        auto tilesNode = loadAndGetNode("test/resources/boards/boardDefinitionPlayerEntities.yaml", "Tiles");
        auto agentsNode = loadAndGetNode("test/resources/boards/boardDefinitionPlayerEntities.yaml", "Agents");
        auto entitiesNode = loadAndGetNode("test/resources/boards/boardDefinitionPlayerEntities.yaml", "Entities");


        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());

        ////Load tiles and board
        parserPtr->parseAgents(agentsNode, *gameConfigPtr);
        parserPtr->parseTileTypes(tilesNode, *gameConfigPtr);
        parserPtr->parseEntities(entitiesNode, *gameConfigPtr);
        parserPtr->parseBoardGenerator(boardNode, *gameConfigPtr);

        auto state = gameConfigPtr->generateGameState();

        ASSERT_EQ(gameConfigPtr->tileTypes.size(), 5);

        auto selectedLevelDefinition = gameConfigPtr->levelDefinitions.find(0);
        auto mockGameStatePtr = std::shared_ptr<MockGameState>(new MockGameState());
        auto tiles = gameConfigPtr->instanceTiles(*mockGameStatePtr, selectedLevelDefinition->second.board);

        ASSERT_EQ(tiles.size(), 130);


        ASSERT_EQ(gameConfigPtr->entityTypes.size(), 4);
        ASSERT_EQ(state->getEntities().size(), 12);
        ASSERT_EQ(state->getNumPlayers(), 2);
    }

    TEST(BoardParseTest, testLoadBoardWithDefinedEntitiesNoSpace)
    {
        auto boardNode = loadAndGetNode("test/resources/boards/boardDefinitionPlayerEntitiesNoSpace.yaml", "Board");
        auto tilesNode = loadAndGetNode("test/resources/boards/boardDefinitionPlayerEntitiesNoSpace.yaml", "Tiles");
        auto agentsNode = loadAndGetNode("test/resources/boards/boardDefinitionPlayerEntitiesNoSpace.yaml", "Agents");
        auto entitiesNode = loadAndGetNode("test/resources/boards/boardDefinitionPlayerEntitiesNoSpace.yaml", "Entities");


        auto gameConfigPtr = std::shared_ptr<GameConfig>(new GameConfig());
        auto parserPtr = std::shared_ptr<GameConfigParser>(new GameConfigParser());

        ////Load tiles and board
        parserPtr->parseAgents(agentsNode, *gameConfigPtr);
        parserPtr->parseTileTypes(tilesNode, *gameConfigPtr);
        parserPtr->parseEntities(entitiesNode, *gameConfigPtr);
        parserPtr->parseBoardGenerator(boardNode, *gameConfigPtr);

        auto state = gameConfigPtr->generateGameState();

        ASSERT_EQ(gameConfigPtr->tileTypes.size(), 5);

        auto selectedLevelDefinition = gameConfigPtr->levelDefinitions.find(0);
        auto mockGameStatePtr = std::shared_ptr<MockGameState>(new MockGameState());
        auto tiles = gameConfigPtr->instanceTiles(*mockGameStatePtr, selectedLevelDefinition->second.board);

        ASSERT_EQ(tiles.size(), 130);


        ASSERT_EQ(gameConfigPtr->entityTypes.size(), 4);
        ASSERT_EQ(state->getEntities().size(), 12);
        ASSERT_EQ(state->getNumPlayers(), 2);
    }
}