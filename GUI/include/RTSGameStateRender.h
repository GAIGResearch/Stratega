#pragma once
#include <GameStateRenderer.h>
#include <Game/RTSGame.h>
#include <Representation/RTSGameState.h>
#include <SFML/Window/Window.hpp>
#include <SFML/Graphics.hpp>
#include <CircularBuffer.h>
#include <RTSUnitLayer.h>
#include <RTSOverlayLayer.h>

class RTSGameStateRender : public GameStateRenderer<SGA::RTSGameState>
{
public:
	//Debug mode
	bool drawDebug = true;
	
	RTSGameStateRender(SGA::RTSGame& game, const std::unordered_map<int, std::string>& tileSprites, const std::unordered_map<int, std::string>& unitSprites, int playerID);
	void run(bool& isRunning) override;

	// GameCommunicator functions
	void onGameStateAdvanced() override;

	bool isSelected(int unitID)
	{
		return selectedUnits.find(unitID) != selectedUnits.end();
	}

private:
	void init() override;
	void init(const std::unordered_map<int, std::string>& tileSprites, const std::unordered_map<int, std::string>& unitSprites);
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
	SGA::RTSGame* game;

	//Current gamestate used to render
	SGA::RTSGameState gameStateCopy;

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
	CircularBuffer<SGA::RTSGameState> gameStatesBuffer;
	int gameStatesBufferRCurrentIndex = 0;

	//Human player
	std::unordered_set<int> selectedUnits;

	//Imgui
	sf::Clock deltaClock;

	std::mutex advanceMutex;
};