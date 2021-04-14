#pragma once
#include <SFML/Graphics.hpp>

#include <AssetCache.h>
#include <TileMap.h>
#include <EntityRenderer.h>

#include <Widgets/FogOfWarController.h>
#include <Widgets/ActionsController.h>

namespace SGA
{
	struct GameState;
	struct GameConfig;
	struct RenderConfig;
	
	enum class InteractiveMode
	{
		Disabled,
		Enabled,
	};

	class NewGameStateRenderer
	{
	public:
		NewGameStateRenderer();
		NewGameStateRenderer(const NewGameStateRenderer& other) = delete;
		NewGameStateRenderer(NewGameStateRenderer&& other) noexcept = delete;
		NewGameStateRenderer& operator=(const NewGameStateRenderer& other) = delete;
		NewGameStateRenderer& operator=(NewGameStateRenderer&& other) noexcept = delete;

		void init(const GameState& initialState, const GameConfig& gameConfig, const RenderConfig& renderConfig);
		void update(const GameState& state);
		void setInteractiveMode(InteractiveMode mode);
		void setPlayerPointOfView(int playerID);

	private:
		void handleInput(const GameState& state);
		void render(const GameState& originalState, const GameState& fowState);
		void renderLayers(const GameState& originalState, const GameState& fowState);

		// Event Handling
		void mouseScrolled(const sf::Event& event);
		void mouseButtonReleased(const sf::Event& event);
		void mouseButtonPressed(const sf::Event& event, const GameState& state);
		void mouseMoved(const sf::Event& event);
		
		// ImGUI
		void createHUD(const GameState& originalState, const GameState& fowState);
		void createWindowInfo(const GameState& originalState, const GameState& fowState);
		void createWindowUnits(const GameState& originalState, const GameState& fowState);
		void createWindowActions(const GameState& originalState, const GameState& fowState);
		void createWindowPlayerParameters(const GameState& originalState, const GameState& fowState);
		void createWindowFogOfWar(const GameState& originalState, const GameState& fowState);
		void createTopBar(const GameState& originalState, const GameState& fowState);
		void createEntityInformation(const GameState& originalState, const GameState& fowState);
		void createActionBar(const GameState& originalState, const GameState& fowState);

		sf::RenderWindow window;
		sf::Clock deltaClock;
		int pointOfViewPlayerID;
		InteractiveMode currentMode;

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