#pragma once
#include <Stratega/GUI/GameRenderer.h>
#include <Stratega/GUI/AssetCache.h>
#include <Stratega/GUI/TileMap.h>
#include <Stratega/GUI/EntityRenderer.h>
#include <Stratega/GUI/Widgets/FogOfWarController.h>
#include <Stratega/GUI/Widgets/ActionsController.h>
#include <Stratega/ForwardModel/ActionAssignment.h>

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	class TBSGameRenderer : public GameRenderer
	{
	public:
		TBSGameRenderer();
		
		void init(const GameState& initialState, const GameConfig& gameConfig) override;
		void update(const GameState& state) override;
		void render() override;
		void setPlayerPointOfView(int playerID) override;
		ActionAssignment getPlayerActions() override;

		bool isActionAvailable() const;
		
	private:
		void updateFow();
		void handleInput();
		void renderLayers();

		// Event Handling
		void mouseScrolled(const sf::Event& event);
		void mouseButtonReleased(const sf::Event& event);
		void mouseButtonPressed(const sf::Event& event);
		void mouseMoved(const sf::Event& event);

		// ImGUI
		void createHUD();
		void createWindowInfo();
		void createWindowUnits();
		void createWindowActions();
		void createWindowPlayerParameters();
		void createWindowFogOfWar();
		void createEntityInformation();
		void createActionBar();

		const GameConfig* config;
		GameState state;
		GameState fowState;
		std::optional<Action> selectedAction;
		
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
		bool dragging;
		sf::Vector2f oldMousePosition;
	};
}
