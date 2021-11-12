#include <Mocks/Stratega/core/MockGrid.hpp>
#include <Mocks/Stratega/core/MockGameRunner.hpp>
#include <Mocks/Stratega/core/MockGameConfig.hpp>
#include <Mocks/Stratega/core/MockForwardModel.hpp>
#include <Mocks/Stratega/core/MockAgent.hpp>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "Stratega/Configuration/GameConfigParser.h"
#include "Stratega/Game/TBSGameRunner.h"
#include <Stratega/core/TestUtils/utils.hpp>

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
    TEST(GameRunnerTest, init)
	{
        //Generate config
        std::string configPath("test/resources/test.yaml");
        auto gameConfig = SGA::loadConfigFromYAML(configPath);

        auto mockForwardModelPtr = std::unique_ptr<MockForwardModel>(new MockForwardModel());    
        auto mockGameConfigPtr = std::shared_ptr<MockGameConfig>(new MockGameConfig());
        mockGameConfigPtr->forwardModel = std::move(mockForwardModelPtr);

        EXPECT_CALL(*mockGameConfigPtr, generateGameState(-1))
            .Times(1);

        EXPECT_CALL(*static_cast<MockForwardModel*>(mockGameConfigPtr->forwardModel.get()), clone())
            .Times(1);

        auto gameRunner = SGA::createGameRunner(*mockGameConfigPtr);

        EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockGameConfigPtr.get()));
        EXPECT_TRUE(Mock::VerifyAndClearExpectations(static_cast<MockForwardModel*>(mockGameConfigPtr->forwardModel.get())));

    }

    TEST(GameRunnerTest, reset)
	{
        //Generate config
        std::string configPath("test/resources/test.yaml");
        auto gameConfig = SGA::loadConfigFromYAML(configPath);

        auto mockForwardModelPtr = std::unique_ptr<MockForwardModel>(new MockForwardModel());    
        auto mockGameConfigPtr = std::shared_ptr<MockGameConfig>(new MockGameConfig());
        mockGameConfigPtr->forwardModel = std::move(mockForwardModelPtr);

        EXPECT_CALL(*mockGameConfigPtr, generateGameState(-1))
            .Times(2);

        EXPECT_CALL(*static_cast<MockForwardModel*>(mockGameConfigPtr->forwardModel.get()), clone())
            .Times(2);

        auto gameRunner = SGA::createGameRunner(*mockGameConfigPtr);

        gameRunner->reset();

        EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockGameConfigPtr.get()));
        EXPECT_TRUE(Mock::VerifyAndClearExpectations(static_cast<MockForwardModel*>(mockGameConfigPtr->forwardModel.get())));

        //auto state = gameRunner->getGameState();
    }

 //   TEST(GameRunnerTest, playAgentsTest)
	//{

 //       std::string configPath("tests/resources/test.yaml");
 //       auto gameConfig = SGA::loadConfigFromYAML(configPath);      

 //       auto mockGameRunnerPtr = std::unique_ptr<MockGameRunner>(new MockGameRunner(*gameConfig));
 //       //Generate Agents
 //       auto yamlString = R"(
 //       Agents:
 //         - RandomAgent
 //         - HumanAgent
 //       )";

 //       auto agentsNode = loadFromStringAndGetNode(yamlString, "Agents");

 //       auto agents = gameConfig->generateAgents();

 //       EXPECT_CALL(*mockGameRunnerPtr, initializeRenderer(Eq(0)))
 //           .Times(1);

 //       EXPECT_CALL(*mockGameRunnerPtr, initializeAgents(_))
 //           .Times(1);

 //       EXPECT_CALL(*mockGameRunnerPtr, ensureRendererInitialized(_))
 //           .Times(1);

 //       EXPECT_CALL(*mockGameRunnerPtr, playInternal(_,0))
 //           .Times(1);

 //       mockGameRunnerPtr->play(agents.begin(), agents.end(), SGA::Vector2f(1200, 800));

 //       EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockGameRunnerPtr.get()));
 //       //auto state = gameRunner->getGameState();
 //   }

 //   TEST(GameRunnerTest, playAgentsCallsTest)
	//{
 //       std::string configPath("../../../tests/resources/test.yaml");
 //       auto gameConfig = SGA::loadConfigFromYAML(configPath);      

 //       auto mockGameRunnerPtr = std::unique_ptr<MockGameRunner>(new MockGameRunner(*gameConfig));

 //       std::vector<std::unique_ptr<MockAgent>> agents;
 //       agents.emplace_back(std::make_unique<MockAgent>("AgentsTest1"));
 //       agents.emplace_back(std::make_unique<MockAgent>("AgentsTest2"));

 //       EXPECT_CALL(*mockGameRunnerPtr, initializeRenderer(Eq(0)))
 //           .Times(1);

 //       EXPECT_CALL(*mockGameRunnerPtr, initializeAgents(_))
 //           .Times(1);

 //       EXPECT_CALL(*mockGameRunnerPtr, ensureRendererInitialized(SGA::Vector2f(1200, 800)))
 //           .Times(1);

 //       EXPECT_CALL(*mockGameRunnerPtr, playInternal(_,0))
 //           .Times(1);

 //       EXPECT_CALL(*agents[0], setPlayerID(0))
 //           .Times(1);
 //       EXPECT_CALL(*agents[1], setPlayerID(1))
 //           .Times(1);

 //       mockGameRunnerPtr->play(agents.begin(), agents.end(), SGA::Vector2f(1200, 800));


 //       EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockGameRunnerPtr.get()));
 //       //auto state = gameRunner->getGameState();
 //   }

 //   TEST(GameRunnerTest, playAgentsWithTwoHumanAgentsTest)
	//{
 //       //Generate Agents
 //       auto yamlString = R"(
 //       Agents:
 //         - HumanAgent
 //         - HumanAgent
 //       )";
 //       auto gameConfigPtr = std::unique_ptr<GameConfig>(new GameConfig());

 //       GameConfigParser parser;
 //       auto agentsNode = loadFromStringAndGetNode(yamlString, "Agents");
 //       parser.parseAgents(agentsNode, *gameConfigPtr);
 //       
 //       auto agents = gameConfigPtr->generateAgents();

 //       auto mockGameRunnerPtr = std::unique_ptr<MockGameRunner>(new MockGameRunner(*gameConfigPtr));     


 //       EXPECT_CALL(*mockGameRunnerPtr, initializeRenderer(Eq(0)))
 //           .Times(1);

 //       EXPECT_CALL(*mockGameRunnerPtr, initializeAgents(_))
 //           .Times(1);

 //       EXPECT_CALL(*mockGameRunnerPtr, ensureRendererInitialized(SGA::Vector2f(1200, 800)))
 //           .Times(1);

 //       EXPECT_CALL(*mockGameRunnerPtr, playInternal(_,0))
 //           .Times(1);

 //       mockGameRunnerPtr->play(agents.begin(), agents.end(), SGA::Vector2f(1200, 800));


 //       EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockGameRunnerPtr.get()));
 //       //auto state = gameRunner->getGameState();
 //   }
}