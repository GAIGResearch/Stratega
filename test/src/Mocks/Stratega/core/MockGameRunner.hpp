#pragma once

#include <Stratega/Game/GameRunner.h>
#include "gmock/gmock.h"
#include <math.h>

namespace SGA {
	class MockGameRunner : public GameRunner
	{
    public:
        MockGameRunner(GameConfig& config) : GameRunner(config) {}

        MOCK_METHOD(void, reset, (), (override));
        MOCK_METHOD(void, reset, (int levelID), (override));

        MOCK_METHOD(void, step, (const ActionAssignment& actions),(override));
        MOCK_METHOD(void, render, (Vector2i& resolution), (override));

        MOCK_METHOD(void, runInternal, (std::vector<Agent*>& agents, GameObserver& observer), (override));
        MOCK_METHOD(void, playInternal, (std::vector<Agent*>& agents, int humanIndex), (override));

        MOCK_METHOD(void, initializeAgents, (std::vector<Agent*>& agents), (override));
        MOCK_METHOD(void, ensureRendererInitialized, (Vector2i& resolution), (override));

        MOCK_METHOD(void, initializeRenderer, (int humanIndex), (override));

        using GameRunner::play;
        /*void initializeAgents(std::vector<Agent*>& agents);
        void ensureRendererInitialized(Vector2f& resolution);*/

       //
       // //std::vector<std::unique_ptr<Agent>>::iterator
       // MOCK_METHOD(void, runMock, (std::vector<std::unique_ptr<Agent>>::iterator begin, std::vector<std::unique_ptr<Agent>>::iterator end, GameObserver* observer));
       //// MOCK_METHOD(void, ensureRendererInitialized, (SGA::Vector2f& resolution), ());
       // 
       // void run(std::vector<std::unique_ptr<Agent>>::iterator begin, std::vector<std::unique_ptr<Agent>>::iterator end, GameObserver* observer = nullptr)
       // {
       //     runMock(begin, end, observer);
       // }
	};
}  