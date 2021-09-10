#pragma once

#include <Stratega/Configuration/GameConfig.h>
#include <gmock/gmock.h>

namespace SGA {
	class MockGameConfig : public GameConfig
	{
    public:
        MOCK_METHOD(size_t, getNumberOfPlayers, (), (const));
        MOCK_METHOD(int, getEntityID, (const std::string& name), (const));
        MOCK_METHOD(int, getTileID, (const std::string& name), (const));
        MOCK_METHOD(int, getActionID, (const std::string& name), (const));
        MOCK_METHOD(int, getTechnologyID, (const std::string& name), (const));

        MOCK_METHOD(int, addPlayer, (std::unique_ptr<GameState>& state, GameInfo& gameInfo), (const));

        MOCK_METHOD(std::vector<std::unique_ptr<Agent>>, generateAgents, (), ());
        MOCK_METHOD(std::unique_ptr<GameState>, generateGameState, (int levelID));

        MOCK_METHOD(void, initializeGamestate, (GameState& state));
        MOCK_METHOD(void, generateGameInfo, (GameState& state));
        MOCK_METHOD(void, addPlayers, (GameState& state));

        //std::unique_ptr<GameState> generateGameState(int levelID = -1) const;

	};

}  