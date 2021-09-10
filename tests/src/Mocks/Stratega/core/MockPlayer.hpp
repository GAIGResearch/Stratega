#pragma once

#include <Stratega/Representation/Player.h>
#include <gmock/gmock.h>

namespace SGA {
	class MockPlayer : public Player
	{
    public:
        MockPlayer(int id, bool canPlay) : Player(id,canPlay) {

        };

       MOCK_METHOD(int, getID, (), ());
       
       /*MOCK_METHOD(int, getEntityID, (const std::string& name), (const));
        MOCK_METHOD(int, getTileID, (const std::string& name), (const));
        MOCK_METHOD(int, getActionID, (const std::string& name), (const));
        MOCK_METHOD(int, getTechnologyID, (const std::string& name), (const));

        MOCK_METHOD(int, addPlayer, (std::unique_ptr<GameState>& state, GameInfo& gameInfo), (const));

        MOCK_METHOD(std::vector<std::unique_ptr<Agent>>, generateAgents, (), ());
        MOCK_METHOD(std::unique_ptr<GameState>, generateGameState, (int levelID));*/

        //std::unique_ptr<GameState> generateGameState(int levelID = -1) const;

	};

}  