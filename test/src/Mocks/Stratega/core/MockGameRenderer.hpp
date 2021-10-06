#pragma once

#include <Stratega/GUI/GameRenderer.h>
#include "gmock/gmock.h"
#include <math.h>

namespace SGA {
	class MockGameRenderer : public GameRenderer
	{
    public:
        MockGameRenderer() : GameRenderer() {}

        MOCK_METHOD(void, setPlayerPointOfView, (int playerID), (override));

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