#pragma once
#include <Stratega/GUI/GameRenderer.h>
#include <Stratega/GUI/AssetCache.h>
#include <Stratega/GUI/TileMap.h>
#include <Stratega/GUI/EntityRenderer.h>
#include <Stratega/GUI/Widgets/FogOfWarController.h>
#include <Stratega/GUI/Widgets/ActionsController.h>
#include <Stratega/NewGUI/Widget.h>
#include <Stratega/NewGUI/World.h>
#include <Stratega/ForwardModel/ActionAssignment.h>
#include <Stratega/NewGUI/SGARenderTarget.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>


namespace SGA
{
	class NewTBSGameRenderer : public GameRenderer
	{
	public:
		NewTBSGameRenderer(SGA::Vector2i& resolution);
		
		void init(const GameState& initialState, const GameConfig& gameConfig) override;
		void update(const GameState& state) override;
		void render() override;
		void setPlayerPointOfView(int playerID) override;
		ActionAssignment getPlayerActions() override;

		bool isActionAvailable() const {
			return false;
		}

		bool isGameEndRequested() override;

		void closeWindow() override
		{
			window.close();
		}
		
	private:
		void updateFow();
		void handleInput();
		void renderLayers();

		// Event Handling
		void mouseScrolled(const sf::Event& event);
		void mouseButtonReleased(const sf::Event& event);
		void mouseButtonPressed(const sf::Event& event);
		void mouseMoved(const sf::Event& event);

		const GameConfig* config;
		GameState state;
		GameState fowState;

		bool endGameRequested = false;
		/*nonstd::optional<Action> selectedAction;
		bool endGameRequested = false;*/
		
		sf::RenderWindow window;
		sf::Clock deltaClock;

		/*
		int pointOfViewPlayerID;
		AssetCache assetCache;
		Widgets::FogOfWarSettings fowSettings;
		Widgets::ActionsSettings actionsSettings;
		TileMap tileMap;
		EntityRenderer entityRenderer;*/

		// Variables for input handling
		float zoomValue;
		bool dragging;
		sf::Vector2f oldMousePosition;

		//New system
		std::vector<std::unique_ptr<SGAWidget>> widgets;
		World world;
		SGARenderTarget renderTarget;
	};
}
