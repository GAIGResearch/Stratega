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
    TEST(BoardTest, checkCorrectSize)
    {
        auto defaultTile=std::shared_ptr<Tile>(new Tile(nullptr, 0, 0));
        auto board = std::shared_ptr<Grid2D<Tile>>(new Grid2D<Tile>(200,150, *defaultTile));
        
        ASSERT_EQ(board->getHeight(), 150);
        ASSERT_EQ(board->getWidth(), 200);

    }

    TEST(BoardTest, checkGridBounds)
    {
        auto defaultTile = std::shared_ptr<Tile>(new Tile(nullptr, 0, 0));
        auto board = std::shared_ptr<Grid2D<Tile>>(new Grid2D<Tile>(200,150, *defaultTile));
        
        ASSERT_EQ(board->isInBounds({200,150}), false);
        ASSERT_EQ(board->isInBounds({199,149}), true);

        ASSERT_EQ(board->isInBounds({0,0}), true);
        ASSERT_EQ(board->isInBounds({-1,-1}), false);
    }

    TEST(BoardTest, getExistingTiles)
    {        
        auto defaultTileType = std::shared_ptr<TileType>(new TileType());
        auto defaultTile = std::shared_ptr<Tile>(new Tile(defaultTileType.get(), 0, 0));
        auto board = std::shared_ptr<Grid2D<Tile>>(new Grid2D<Tile>(200,150, *defaultTile));
        
        ASSERT_EQ(board->get(15,15).name(), "");
    }

    TEST(BoardTest, getNonExistingTiles)
    {        
        auto defaultTileType = std::shared_ptr<TileType>(new TileType());
        auto defaultTile = std::shared_ptr<Tile>(new Tile(defaultTileType.get(), 0, 0));
        auto board = std::shared_ptr<Grid2D<Tile>>(new Grid2D<Tile>(200,150, *defaultTile));        
        
        EXPECT_THROW(board->get(200, 200), std::runtime_error);        
    }
}