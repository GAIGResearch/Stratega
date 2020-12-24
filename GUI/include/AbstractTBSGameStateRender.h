#pragma once
#include <GameStateRenderer.h>
#include <Game/AbstractGame.h>
#include <Representation/TBSGameState.h>
#include <Representation/TBSGameState.h>
#include <SFML/Window/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <CircularBuffer.h>

#include <ForwardModel/Action.h>

class AbstractTBSGameStateRender : public GameStateRenderer<SGA::TBSGameState2>
{
public:
	AbstractTBSGameStateRender(SGA::AbstractTBSGame& game, const std::unordered_map<int, std::string>& tileSprites, const std::map<std::string, std::string>& entitySpritePaths, int playerID);
	void run(bool& isRunning) override;

	// GameCommunicator functions
	void onGameStateAdvanced() override;

private:
	void init() override;
	void init(const std::unordered_map<int, std::string>& tileSprites, const std::map<std::string, std::string>& entitySpritePaths);
	void initializeView(sf::RenderWindow& window) const;
	void initializeLayers();

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

	void createWindowInfo()const;
	void createWindowUnits();
	void createWindowActions();
	void createWindowMultipleActions(sf::RenderWindow& window);


	//Human player stuff
	void waitForHumanToPlay()
	{
		waitForAction = true;

		actionsHumanCanPlay = game->getForwardModel().generateActions(gameStateCopy,getPlayerID());
		actionHumanUnitSelected.clear();
	}

	void playAction(SGA::Action action)
	{
		waitForAction = false;
		actionsHumanCanPlay.clear();
		actionHumanUnitSelected.clear();
		selectedEntity = nullptr;
		showMultipleActions = false;
		game->addActionToExecute(action);
	}

private:

	//Game Data
	SGA::AbstractTBSGame* game;

	//Current gamestate used to render
	SGA::TBSGameState2 gameStateCopy;

	//Zoom
	float zoomValue = 5;

	//Mouse Information
	SGA::Vector2f currentMousePos;
	sf::Vector2f oldPos;
	bool moving = false;

	//FPS
	int fpsLimit = 60;
	int m_fps = 0;

	//Drawing gameState Buffer
	bool drawGameStateBuffer = false;
	CircularBuffer<SGA::TBSGameState2> gameStatesBuffer;
	int gameStatesBufferRCurrentIndex = 0;

	//Human player
	bool waitForAction = false;
	std::vector<SGA::Action> actionsHumanCanPlay;

	std::vector<SGA::Action> actionHumanUnitSelected;
	SGA::Entity* selectedEntity{};

	//Imgui
	sf::Clock deltaClock;

	//If a tile has more than one action, we render a window
	//to show all the action that can be played in that tile
	bool showMultipleActions = false;
	SGA::Vector2i multipleActionsSourceTile;
};
