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
    TEST(GameStateTest, testAddPlayer)
    {
        auto entityTypePtr = std::shared_ptr<EntityType>(new EntityType());
        auto fmPtr = std::shared_ptr<TBSForwardModel>(new TBSForwardModel());
        auto mockPlayerPtr = std::shared_ptr<MockPlayer>(new MockPlayer(0, true));

        auto gameState = std::shared_ptr<GameState>(new GameState());
        auto position = Vector2f{ 10,0 };

        gameState->addPlayer(*mockPlayerPtr);

        ASSERT_TRUE(gameState->getPlayer(0) != nullptr);
        ASSERT_EQ(gameState->getNumPlayers(), 1);
        ASSERT_EQ(gameState->getPlayers().size(), 1);

    }

    TEST(GameStateTest, testAddEntity)
    {
        auto entityTypePtr = std::shared_ptr<EntityType>(new EntityType());
        auto fmPtr = std::shared_ptr<TBSForwardModel>(new TBSForwardModel());
        
        auto gameState = std::shared_ptr<GameState>(new GameState());
        auto position = Vector2f{ 10,0 };

        fmPtr->spawnEntity(*gameState, *entityTypePtr, -1, position);
        
        ASSERT_TRUE(gameState->getEntity(0)!=nullptr);
        ASSERT_TRUE(gameState->getEntityAt(position) !=nullptr);
        ASSERT_EQ(gameState->getEntities().size(), 1);        

        EXPECT_THROW(gameState->getPlayerEntities(0).size(), std::runtime_error);
        EXPECT_THROW(gameState->getPlayerEntities(1).size(), std::runtime_error);
    }

    TEST(GameStateTest, testAddEntityWithPlayerID)
    {
        auto entityTypePtr = std::shared_ptr<EntityType>(new EntityType());
        auto fmPtr = std::shared_ptr<TBSForwardModel>(new TBSForwardModel());
        
        auto gameState = std::shared_ptr<GameState>(new GameState());
        auto position = Vector2f{ 10,0 };

        fmPtr->spawnEntity(*gameState, *entityTypePtr, 0, position);
        

        ASSERT_TRUE(gameState->getEntity(0)!=nullptr);
        ASSERT_TRUE(gameState->getEntityAt(position) !=nullptr);
        ASSERT_EQ(gameState->getEntities().size(), 1);

        auto mockPlayerPtr = std::shared_ptr<MockPlayer>(new MockPlayer(0, true));
        auto mockPlayerPtr2 = std::shared_ptr<MockPlayer>(new MockPlayer(1, true));

        gameState->addPlayer(*mockPlayerPtr);
        gameState->addPlayer(*mockPlayerPtr2);        

        ASSERT_EQ(gameState->getPlayerEntities(0).size(), 1);
        ASSERT_EQ(gameState->getPlayerEntities(1).size(), 0);

        EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockPlayerPtr.get()));
    }

    TEST(GameStateTest, testRemoveEntityWithPlayerID)
    {
        auto entityTypePtr = std::shared_ptr<EntityType>(new EntityType());
        auto fmPtr = std::shared_ptr<TBSForwardModel>(new TBSForwardModel());
        
        auto gameState = std::shared_ptr<GameState>(new GameState());
        auto position = Vector2f{ 10,0 };

        auto mockPlayerPtr = std::shared_ptr<MockPlayer>(new MockPlayer(0, true));
        auto mockPlayerPtr2 = std::shared_ptr<MockPlayer>(new MockPlayer(1, true));
        gameState->addPlayer(*mockPlayerPtr);
        gameState->addPlayer(*mockPlayerPtr2);

        fmPtr->spawnEntity(*gameState, *entityTypePtr, 0, position);                

        ASSERT_TRUE(gameState->getEntity(0)!=nullptr);
        ASSERT_TRUE(gameState->getEntityAt(position) !=nullptr);
        ASSERT_EQ(gameState->getEntities().size(), 1);
        
        ASSERT_EQ(gameState->getPlayerEntities(0).size(), 1);
        ASSERT_EQ(gameState->getPlayerEntities(1).size(), 0); 


        EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockPlayerPtr.get()));
        EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockPlayerPtr2.get()));

        //Remove entity
        auto entity0 = gameState->getEntity(0);
        entity0->flagRemove();
        ASSERT_EQ(entity0->flagged(), true);

        //Advance gm
        fmPtr->advanceGameState(*gameState, ActionAssignment::createEndActionAssignment(0));

        ASSERT_TRUE(gameState->getEntity(0) == nullptr);
        ASSERT_TRUE(gameState->getEntityAt(position) == nullptr);
        ASSERT_EQ(gameState->getEntities().size(), 0);
        ASSERT_EQ(gameState->getPlayerEntities(0).size(), 0);
        ASSERT_EQ(gameState->getPlayerEntities(1).size(), 0);

    }

    TEST(GameStateTest, testRemoveEntityWithPlayerIDRepeteadly)
    {
        auto entityTypePtr = std::shared_ptr<EntityType>(new EntityType());
        auto fmPtr = std::shared_ptr<TBSForwardModel>(new TBSForwardModel());
        
        auto gameState = std::shared_ptr<GameState>(new GameState());
        auto position = Vector2f{ 10,0 };

        auto mockPlayerPtr = std::shared_ptr<MockPlayer>(new MockPlayer(0, true));
        auto mockPlayerPtr2 = std::shared_ptr<MockPlayer>(new MockPlayer(1, true));
        gameState->addPlayer(*mockPlayerPtr);
        gameState->addPlayer(*mockPlayerPtr2);

        fmPtr->spawnEntity(*gameState, *entityTypePtr, 0, position);
        
        ASSERT_TRUE(gameState->getEntity(0)!=nullptr);
        ASSERT_TRUE(gameState->getEntityAt(position) !=nullptr);
        ASSERT_EQ(gameState->getEntities().size(), 1);
        ASSERT_EQ(gameState->getPlayerEntities(0).size(), 1);
        ASSERT_EQ(gameState->getPlayerEntities(1).size(), 0); 

        //Remove entity
        auto entity0 = gameState->getEntity(0);
        entity0->flagRemove();
        ASSERT_EQ(entity0->flagged(), true);

        //Advance gm
        fmPtr->advanceGameState(*gameState, ActionAssignment::createEndActionAssignment(0) );

        ASSERT_TRUE(gameState->getEntity(0) == nullptr);
        ASSERT_TRUE(gameState->getEntityAt(position) == nullptr);
        ASSERT_EQ(gameState->getEntities().size(), 0);
        ASSERT_EQ(gameState->getPlayerEntities(0).size(), 0);
        ASSERT_EQ(gameState->getPlayerEntities(1).size(), 0);   


        //Add entities
        for (size_t i = 0; i < 4; i++)
        {
            fmPtr->spawnEntity(*gameState, *entityTypePtr, 0, {0,static_cast<double>(i)});
        }
        
        //Check we dont override entities IDs
        ASSERT_TRUE(gameState->getEntity(0) == nullptr);
        ASSERT_TRUE(gameState->getEntityAt(position) == nullptr);

        ASSERT_EQ(gameState->getEntities().size(), 4);
        ASSERT_EQ(gameState->getPlayerEntities(0).size(), 4);
        ASSERT_EQ(gameState->getPlayerEntities(1).size(), 0);

        for (size_t i = 0; i < 4; i++)
        {
            ASSERT_TRUE(gameState->getEntity(i+1) != nullptr);
            ASSERT_TRUE(gameState->getEntityAt({ 0,static_cast<double>(i) }) != nullptr);
        }
    }

    TEST(GameStateTest, moveEntities)
    {
        auto entityTypePtr = std::shared_ptr<EntityType>(new EntityType());
        auto fmPtr = std::shared_ptr<TBSForwardModel>(new TBSForwardModel());
        
        auto gameState = std::shared_ptr<GameState>(new GameState());
        auto position = Vector2f{ 10,0 };
        auto newPosition = Vector2f{ 10,1 };

        auto mockPlayerPtr = std::shared_ptr<MockPlayer>(new MockPlayer(0, true));
        gameState->addPlayer(*mockPlayerPtr);

        fmPtr->spawnEntity(*gameState, *entityTypePtr, 0, position);

        ASSERT_TRUE(gameState->getEntityAt(position) != nullptr);

        auto* entity0 = gameState->getEntity(0);

        entity0->setPosition(newPosition);
        
        ASSERT_TRUE(gameState->getEntityAt(position) == nullptr);
        ASSERT_TRUE(gameState->getEntityAt(newPosition) != nullptr);
    }
}