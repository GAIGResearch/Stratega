#pragma once

#include <Stratega/ForwardModel/ForwardModel.h>
#include <gmock/gmock.h>

namespace SGA {
	class MockForwardModel : public ForwardModel
	{
    public:
        MockForwardModel() : ForwardModel()
        {

        }

        MOCK_METHOD(std::unique_ptr<ForwardModel>, clone, (), (const, override));
        MOCK_METHOD(void , advanceGameState, (GameState& state, const Action& action), (const, override));
        MOCK_METHOD(void , advanceGameState, (GameState& state, const ActionAssignment& action), (const, override));
        MOCK_METHOD(GameType , getGameType, (), (const, override));
        //MOCK_METHOD(int, getEntityID, (const std::string& name), (const));
        //MOCK_METHOD(int, getTileID, (const std::string& name), (const));
        //MOCK_METHOD(int, getActionID, (const std::string& name), (const));
        //MOCK_METHOD(int, getTechnologyID, (const std::string& name), (const));
        //
        //MOCK_METHOD(int, addPlayer, (std::unique_ptr<GameState>& state, GameInfo& gameInfo), (const));
        //
        //MOCK_METHOD(std::vector<std::unique_ptr<Agent>>, generateAgents, (), ());
        //MOCK_METHOD(std::unique_ptr<GameState>, generateGameState, () ,(const, override));
        //MOCK_METHOD(std::unique_ptr<GameState>, generateGameState, (int levelID),(const, override));

        //MOCK_METHOD(void, initializeGamestate, (GameState& state));
        //MOCK_METHOD(void, generateGameInfo, (GameState& state));
        //MOCK_METHOD(void, addPlayers, (GameState& state));

        //std::unique_ptr<GameState> generateGameState(int levelID = -1) const;

	};

}  