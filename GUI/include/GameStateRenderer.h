#pragma once
#include <Game/GameCommunicator.h>
#include <thread>
#include <AssetCache.h>
#include <mutex>

#include <SFML/Window/Window.hpp>
#include <SFML/Graphics.hpp>


#include <MapLayer.h>
#include <UnitLayer.h>
#include <OverlayLayer.h>

#include <imgui-SFML.h>
#include <imgui.h>
#include <iomanip>

#include <Game/Game.h>

class GameStateRenderBase : public SGA::GameCommunicator
{
public:
	GameStateRenderBase(int playerID) :
		GameCommunicator{ playerID }
	{
	}
	virtual ~GameStateRenderBase() = default;
	virtual void render() = 0;
};

template<typename GameState>
class GameStateRenderer : public GameStateRenderBase
{
public:
	GameStateRenderer(int playerID) :
		GameStateRenderBase{ playerID }
	{
	}
	virtual ~GameStateRenderer()=default;
	void render() override = 0;
	
	// GameCommunicator functions
	void init() override
	{
		//Initialization
		isRunning = true;
	}
	
	void close() override
	{
		isRunning = false;
	}
	
	void onGameStateAdvanced() override = 0;

	//RenderThread
	std::thread renderThread;
	std::mutex mutexRender;

	//OpenGL context
	sf::Context ctx;

	AssetCache assetCache;

	std::vector<std::unique_ptr<RenderLayer<GameState>>> renderLayers;

protected:
	bool isRunning = false;
};
