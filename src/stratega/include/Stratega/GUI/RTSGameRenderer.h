#pragma once

#include <Stratega/GUI/AssetCache.h>
#include <Stratega/GUI/TileMap.h>
#include <Stratega/GUI/EntityRenderer.h>
#include <Stratega/GUI/Widgets/FogOfWarController.h>
#include <Stratega/GUI/Widgets/ActionsController.h>

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <Stratega/Utils/filesystem.hpp>

#include <Stratega/GUI/GameRenderer.h>
#include <Stratega/ForwardModel/ActionAssignment.h>

namespace SGA
{

	class RTSGameRenderer : public GameRenderer
	{
	public:
		RTSGameRenderer(SGA::Vector2i& resolution);
		
		void init(const GameState& initialState, const GameConfig& gameConfig) override;
		void update(const GameState& state) override;
		void render() override;
		void setPlayerPointOfView(int playerID) override;
		ActionAssignment getPlayerActions() override;
		bool isGameEndRequested() override;

		bool isWaiting() const override
		{
			return false;
		}

		void closeWindow() override
		{
			window.close();
		}
		
	private:
		void handleInput();
		void renderLayers();
		
		//Events Implementation
		void windowClosed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)const;
		void mouseScrolled(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
		void mouseButtonReleased(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
		void mouseButtonPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
		void mouseMoved(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
		void keyPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window);

		//HUD
		void createHUD();
		void createTopBar();
		void createBottomBar();
		void createWindowInfo();
		void createWindowUnits();
		void createWindowActionList();
		void createWindowNavMesh();
		void createWindowFogOfWar();
		void createWindowPlayerParameters() const;
		
		GameState state;
		GameState fowState;
		const GameConfig* config;
		ActionAssignment assignment;
		bool endGameRequested = false;

		bool drawDebug = false;
		
		sf::RenderWindow window;
		sf::Clock deltaClock;
		int pointOfViewPlayerID;
		AssetCache assetCache;
		Widgets::FogOfWarSettings fowSettings;
		Widgets::ActionsSettings actionsSettings;
		TileMap tileMap;
		EntityRenderer entityRenderer;
		
		// Variables for input handling
		float zoomValue;
		bool moving = false;
		bool dragging = false;
		sf::Vector2f oldMousePosition;

		//Collect actions played by previous frame
		std::vector<Action> futureActionsToPlay;
	};
}
