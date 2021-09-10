#pragma once

#include <Stratega/Game/GameRunner.h>
#include "gmock/gmock.h"
#include <math.h>

namespace SGA {
	class MockGameRunner : public GameRunner
	{
    public:
        MockGameRunner(GameConfig& config) : GameRunner(config) {}

        MOCK_METHOD(void, reset, (), ());
        MOCK_METHOD(void, reset, (int levelID), ());

        MOCK_METHOD(void, step, (ActionAssignment& actions),());
        MOCK_METHOD(void, render, (Vector2f& resolution), ());

        MOCK_METHOD(void, runInternal, (std::vector<Agent*>& agents, GameObserver& observer), ());
        MOCK_METHOD(void, playInternal, (std::vector<Agent*>& agents, int humanIndex), ());

        MOCK_METHOD(void, ensureRendererInitialized, (SGA::Vector2f& resolution), ());

	};
}  