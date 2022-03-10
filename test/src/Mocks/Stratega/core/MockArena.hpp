#pragma once

#include <Stratega/Arena/Arena.h>
#include <gmock/gmock.h>

namespace SGA {
	class MockArena : public Arena
	{
    public:
        MockArena(const SGA::GameConfig& config) : Arena(config)
        {

        }

        MOCK_METHOD(void, runGame, (const std::vector<int>& agentAssignment, boost::mt19937 rngEngine), ( override));

       /*  MOCK_METHOD(void , advanceGameState, (GameState& state, const Action& action), (const, override));
        MOCK_METHOD(void , advanceGameState, (GameState& state, const ActionAssignment& action), (const, override));
        MOCK_METHOD(GameType , getGameType, (), (const, override));*/
       
	};

}  