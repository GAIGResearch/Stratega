#pragma once
#include <GameStateRenderer.h>
#include <Game/AbstractRTSGame.h>
#include <Representation/AbstractGS/RTSGameState2.h>
#include <SFML/Window/Window.hpp>
#include <SFML/Graphics.hpp>
#include <CircularBuffer.h>

class AbstractRTSGameStateRender : public GameStateRenderer<SGA::RTSGameState2>
{
public:
	//Debug mode
	bool drawDebug = false;

	AbstractRTSGameStateRender(SGA::AbstractRTSGame& game, const std::unordered_map<int, std::string>& tileSprites, const std::map<std::string, std::string>& entitySpritePaths, int playerID);
	void run(bool& isRunning) override;

	// GameCommunicator functions
	void onGameStateAdvanced() override;

	bool isSelected(int unitID)
	{
		return selectedUnits.find(unitID) != selectedUnits.end();
	}

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

	void createWindowInfo();
	void createWindowUnits();
	void createWindowNavMesh();

private:
	//Game Data
	SGA::AbstractRTSGame* game;

	//Current gamestate used to render
	SGA::RTSGameState2 gameStateCopy;

	//Zoom
	float zoomValue = 5;

	//Mouse Information
	SGA::Vector2f currentMousePos;
	sf::Vector2f oldPos;
	bool moving = false;
	bool dragging = false;

	//FPS
	int fpsLimit = 60;
	int m_fps = 0;

	//Drawing gameState Buffer
	bool drawGameStateBuffer = false;
	CircularBuffer<SGA::RTSGameState2> gameStatesBuffer;
	int gameStatesBufferRCurrentIndex = 0;

	//Human player
	std::unordered_set<int> selectedUnits;

	//Imgui
	sf::Clock deltaClock;

	std::mutex advanceMutex;

	//Profiling
	//Last call OnAdvancedGameGameState
	int indexAdvancedGameState = 0;

	SGA::NavigationConfig config;

	std::vector<sf::Text> unitsInfo;
	std::vector<sf::RectangleShape> healthBars;

};