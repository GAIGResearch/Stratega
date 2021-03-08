#pragma once
#include <GameStateRenderer.h>
#include <Stratega/Game/TBSGame.h>
#include <Stratega/Representation/TBSGameState.h>
#include <SFML/Window/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <CircularBuffer.h>

#include <Stratega/ForwardModel/Action.h>

namespace SGA
{
	struct RenderConfig;
	struct GameConfig;
	
	class TBSGameStateRender : public GameStateRenderer<TBSGameState>
	{
	public:
		TBSGameStateRender(TBSGame& game, const GameConfig& gameConfig, const RenderConfig& renderConfig, int playerID);
		void render() override;

		// GameCommunicator functions
		void onGameStateAdvanced() override;

	private:
		void init() override;
		void init(const GameConfig& gameConfig, const RenderConfig& renderConfig);
		void initializeView(sf::RenderWindow& window) const;
		
		void handleInput(sf::RenderWindow& window);

		//Events Implementation
		void windowClosed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)const;
		void mouseScrolled(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
		void mouseButtonReleased(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
		void mouseButtonPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
		void mouseMoved(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
		void keyPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window);

		//Render Layers by order
		void drawLayers(sf::RenderWindow& window);

		//HUD
		void createHUD(sf::RenderWindow& window);

		void createWindowInfo() const;
		void createWindowUnits();
		void createWindowActions();
		void createWindowPlayerParameters() const;
		void createWindowFogOfWar();
		void createTopBar() const;
		void createEntityInformation(sf::RenderWindow& window);
		void createActionBar(sf::RenderWindow& window);
		

		//Human player stuff
		void waitForHumanToPlay()
		{
			waitForAction = true;
			
			//Update actions
			actionsSettings.actionsHumanPlayer = game->getForwardModel().generateActions(gameStateCopyFogOfWar, getPlayerID());
		}

		void playAction(Action action)
		{
			waitForAction = false;
			actionsSettings.reset();
			game->addActionToExecute(action);
		}
		
	private:

		//Game Data
		TBSGame* game;

		//Current gamestate used to render
		TBSGameState gameStateCopy;
		TBSGameState gameStateCopyFogOfWar;

		//Zoom
		float zoomValue = 5;

		//Mouse Information
		Vector2f currentMousePos;
		sf::Vector2f oldPos;
		bool moving = false;

		//FPS
		int fpsLimit = 60;
		int m_fps = 0;

		//Drawing gameState Buffer
		bool drawGameStateBuffer = false;
		CircularBuffer<TBSGameState> gameStatesBuffer;
		int gameStatesBufferRCurrentIndex = 0;

		//Human player
		bool waitForAction = false;

		//Imgui
		sf::Clock deltaClock;

		//If a tile has more than one action, we render a window
		//to show all the action that can be played in that tile
		bool showMultipleActions = false;
		Vector2i multipleActionsSourceTile;

		//Temp
		int selectedActionType = -1;
	};

}