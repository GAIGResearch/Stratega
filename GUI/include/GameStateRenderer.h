#pragma once
#include <Stratega/Game/GameCommunicator.h>
#include <thread>
#include <AssetCache.h>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <Stratega/Game/Game.h>

#include <Widgets/FogOfWarController.h>
#include <Widgets/ActionsController.h>

namespace SGA
{
	class GameStateRenderBase : public SGA::GameCommunicator
	{
	public:
		GameStateRenderBase(int playerID) :
			GameCommunicator{ playerID }
		{
		}
		virtual ~GameStateRenderBase() = default;
		virtual void render() = 0;
		
		//OutLine Shader
		sf::Shader outLineShadeR;
	};

	template<typename GameState>
	class GameStateRenderer : public GameStateRenderBase
	{
	public:
		GameStateRenderer(int playerID) :
			GameStateRenderBase{ playerID },
			fowSettings{true, Widgets::FogRenderType::Fog, playerID == -1 ? 0 : playerID}
		{
		}
		virtual ~GameStateRenderer() = default;

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
		std::vector<sf::Color> playerColors;
		
		bool isRendering = false;
		Widgets::FogOfWarSettings fowSettings;
		Widgets::ActionsSettings actionSettings;
	};

}