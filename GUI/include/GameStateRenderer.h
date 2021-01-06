#pragma once
#include <Game/GameCommunicator.h>
#include <thread>
#include <AssetCache.h>
#include <mutex>

#include <SFML/Window/Window.hpp>
#include <SFML/Graphics.hpp>


#include <imgui-SFML.h>
#include <imgui.h>
#include <iomanip>

#include <Game/Game.h>
#include<PlayerSelector.h>

class GameStateRenderBase : public SGA::GameCommunicator
{
public:
	PlayerSelector playerSelector;
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
	
	// GameCommunicator functions
	void init() override
	{
		isRendering = true;
	}
	
	void close() override
	{
		isRendering = false;
	}

	void onGameStateAdvanced() override = 0;

	//RenderThread
	std::thread renderThread;
	std::mutex mutexRender;

	//OpenGL context
	sf::Context ctx;

	AssetCache assetCache;

protected:
	//New render system(withoutlayers)
	std::vector<sf::Sprite> mapSprites;
	std::vector<sf::Sprite> entitySprites;
	std::vector<sf::Text> entityInfo;
	std::vector<sf::Sprite> overlaySprites;
	std::vector<sf::CircleShape> actionsSelectedEntity;

	bool isRendering = false;
	bool isFogOfWarActive = false;
	bool renderFogOfWarTile = false;
};
