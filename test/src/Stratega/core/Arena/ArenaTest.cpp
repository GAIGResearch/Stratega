#include <Mocks/Stratega/core/MockGrid.hpp>
#include <Mocks/Stratega/core/MockGameRunner.hpp>
#include <Mocks/Stratega/core/MockGameConfig.hpp>
#include <Mocks/Stratega/core/MockForwardModel.hpp>
#include <Mocks/Stratega/core/MockArena.hpp>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "Stratega/Configuration/GameConfigParser.h"
#include "Stratega/Game/TBSGameRunner.h"
#include "Stratega/Arena/Arena.h"
#include <Stratega/Arena/utils.h>

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
    TEST(ArenaTest, init)
    {
        //Generate config
        std::string configPath("test/resources/test.yaml");
        auto gameConfig = SGA::loadConfigFromYAML(configPath);
        auto mockGameConfigPtr = std::shared_ptr<MockGameConfig>(new MockGameConfig());
        auto mockForwardModelPtr = std::unique_ptr<MockForwardModel>(new MockForwardModel());

        mockGameConfigPtr->forwardModel = std::move(mockForwardModelPtr);


        EXPECT_CALL(*mockGameConfigPtr, generateGameState(-1))
            .Times(1);
        
        EXPECT_CALL(*static_cast<MockForwardModel*>(mockGameConfigPtr->forwardModel.get()), clone())
          .Times(1);

        auto mockGameRunnerPtr = std::shared_ptr<MockGameRunner>(new MockGameRunner(*mockGameConfigPtr));


        EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockGameConfigPtr.get()));
        EXPECT_TRUE(Mock::VerifyAndClearExpectations(static_cast<MockForwardModel*>(mockGameConfigPtr->forwardModel.get())));
    }

    TEST(ArenaTest, combinationGenerationCallsTest)
    {
        auto mockGameConfigPtr = std::shared_ptr<MockGameConfig>(new MockGameConfig());
        auto mockForwardModelPtr = std::unique_ptr<MockForwardModel>(new MockForwardModel());

        mockGameConfigPtr->forwardModel = std::move(mockForwardModelPtr);
        
       
        EXPECT_CALL(*mockGameConfigPtr, generateGameState(_))
            .Times(1);
        EXPECT_CALL(*static_cast<MockForwardModel*>(mockGameConfigPtr->forwardModel.get()), clone())
            .Times(1);

        auto mockArenaPtr = std::shared_ptr<MockArena>(new MockArena(*mockGameConfigPtr));

        EXPECT_CALL(*mockArenaPtr, runGame(_, _))
            .Times(2);

        boost::mt19937 rngEngine(0);
        CallbackFn callback = [&](const std::vector<int>& c) {mockArenaPtr->runGame(c, rngEngine); };       

        generateCombinations(2, 2, callback);

        EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockArenaPtr.get()));
        EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockGameConfigPtr.get()));
        EXPECT_TRUE(Mock::VerifyAndClearExpectations(static_cast<MockForwardModel*>(mockGameConfigPtr->forwardModel.get())));
    }

    //TEST(ArenaTest, runGameTest)
    //{
    //    auto mockGameConfigPtr = std::shared_ptr<MockGameConfig>(new MockGameConfig());
    //    auto mockForwardModelPtr = std::unique_ptr<MockForwardModel>(new MockForwardModel());

    //    mockGameConfigPtr->forwardModel = std::move(mockForwardModelPtr);

    //    EXPECT_CALL(*mockGameConfigPtr, generateGameState(_))
    //        .Times(1);
    //    EXPECT_CALL(*static_cast<MockForwardModel*>(mockGameConfigPtr->forwardModel.get()), clone())
    //        .Times(1);

    //    //auto mockArenaPtr = std::shared_ptr<MockArena>(new MockArena(*mockGameConfigPtr));
    //    auto mockGameRunnerPtr = std::unique_ptr<MockGameRunner>(new MockGameRunner(*mockGameConfigPtr));

    //    Arena arena(*mockGameConfigPtr,std::move(mockGameRunnerPtr));

    //    boost::mt19937 rngEngine(0);
    //    arena.runGame({}, rngEngine);    

    //    //EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockArenaPtr.get()));
    //    EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockGameConfigPtr.get()));
    //    EXPECT_TRUE(Mock::VerifyAndClearExpectations(static_cast<MockForwardModel*>(mockGameConfigPtr->forwardModel.get())));
    //}
}