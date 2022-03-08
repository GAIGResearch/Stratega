#pragma once
#include <Stratega/ForwardModel/ActionAssignment.h>

#include <Stratega/GUI/GameRenderer.h>

#include <Stratega/NewGUI/Widget.h>
#include <Stratega/NewGUI/World.h>

#include <Stratega/ForwardModel/ActionAssignment.h>

#include <Stratega/NewGUI/SGARenderTarget.h>

#include <Stratega/NewGUI/FOWControllerWidget.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>


namespace SGA
{
	class ResourceManager;

	class GenericGameRenderer : public GameRenderer
	{
	public:
		GenericGameRenderer(SGA::Vector2i& resolution);
		
		void init(const GameState& initialState, const GameConfig& gameConfig) override;
		void update(const GameState& state) override;
		void render() override;
		void setPlayerPointOfView(int newPlayerID) override;
		ActionAssignment getPlayerActions() override;

		bool isActionAvailable() override
		{
			return temp.getAssignmentCount() > 0;
			//return selectedAction.has_value();
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
		void keyPressed(const sf::Event& event);

		//Game configuration
		const GameConfig* config;

		//Gamestates
		GameState state;
		GameState fowState;

		bool endGameRequested = false;
		
		//Window and delta time clok
		sf::RenderWindow window;
		sf::Clock deltaClock;

		//Actions to play
		ActionAssignment temp;
		ActionAssignment futureActionsToPlay;

		// Variables for input handling
		float zoomValue;
		bool dragging;
		sf::Vector2f oldMousePosition;

		//New system
		std::vector<std::unique_ptr<SGAWidget>> widgets;
		World world;
		std::unique_ptr<SGARenderTarget> renderTarget;
		std::unique_ptr<ResourceManager> resourceManager;
		int playerID;

		//FOW settings
		FogOfWarSettings settings;
	};
}
