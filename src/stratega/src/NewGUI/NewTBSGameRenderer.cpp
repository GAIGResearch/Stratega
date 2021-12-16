#include <Stratega/NewGUI/NewTBSGameRenderer.h>
#include <Stratega/NewGUI/ResourceManager.h>
#include <Stratega/GUI/GridUtils.h>
#include <Stratega/Configuration/RenderConfig.h>
#include <Stratega/Configuration/GameConfig.h>

#include <SFML/Window.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iomanip>
#include <sstream>
#include <Stratega/Utils/filesystem.hpp>

#include<Stratega/Utils/warnings.h>
#include <Stratega/NewGUI/World.h>
#include <Stratega/NewGUI/GridLayoutWidget.h>

DISABLE_WARNING_PUSH
#if defined(__clang__)    
DISABLE_WARNING_FORMAT
#elif defined(__GNUC__)
DISABLE_WARNING_FORMAT
#endif

namespace SGA
{
	NewTBSGameRenderer::NewTBSGameRenderer(SGA::Vector2i& resolution)
		: config(nullptr),
		state(),
		fowState(),
		window(sf::VideoMode(static_cast<unsigned>(resolution.x), static_cast<unsigned>(resolution.y)), "Stratega GUI", sf::Style::Default | sf::Style::Titlebar),
		zoomValue(5.f),
		dragging(false)
		//world(World::createIsometricGrid(tileWidth, tileHeight))
	{
		// Initialize View
		sf::View view = window.getView();
		view.setCenter(static_cast<float>(window.getSize().x / 2.), static_cast<float>(window.getSize().y / 2.));
		view.setSize(window.getDefaultView().getSize()); // Reset the size
		view.zoom(zoomValue); // Apply the zoom level (this transforms the view)
		window.setView(view);

		//Add layout widget
		widgets.emplace_back( std::make_unique<GridLayoutWidget>(world));
	}

	void NewTBSGameRenderer::init(const GameState& initialState, const GameConfig& gameConfig)
	{
		config = &gameConfig;

		world = World::createIsometricGrid(256, 144, { initialState.getBoardWidth(), initialState.getBoardHeight() });
		resourceManager = ResourceManager::constructFromConfig(gameConfig);
		renderTarget = std::make_unique<SGARenderTarget>(window, *resourceManager, world);
		ImGui::SFML::Init(window);

		update(initialState);
	}

	ActionAssignment NewTBSGameRenderer::getPlayerActions()
	{
		ActionAssignment returnValue;
		
		return returnValue;
	}

	void NewTBSGameRenderer::update(const GameState& newState)
	{
		this->state = newState;
		
		//Update world
		world.update(newState);
		//Render widgets
		for (auto& widget : widgets)
		{
			widget->update(newState);
		}
	}

	void NewTBSGameRenderer::handleInput()
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered())
				continue;

			switch (event.type)
			{
			case sf::Event::Closed: { break; }
			case sf::Event::MouseWheelScrolled: { mouseScrolled(event); break; }
			case sf::Event::MouseButtonReleased: { mouseButtonReleased(event); break; }
			case sf::Event::MouseButtonPressed: { mouseButtonPressed(event); break; }
			case sf::Event::MouseMoved: { mouseMoved(event); 	break; }
									  //case sf::Event::KeyPressed: {keyPressed(event); break;	}
			default:  break;
			}
		}
	}

	bool NewTBSGameRenderer::isGameEndRequested()
	{
		return endGameRequested;
	}

	void NewTBSGameRenderer::render()
	{
		handleInput();

		window.clear();

		//Render world
		world.render(*renderTarget);

		ImGui::SFML::Update(window, deltaClock.restart());
		
		//Render widgets
		for (auto& widget : widgets)
		{
			widget->render(*renderTarget);
		}

		auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		sf::VertexArray selectedTile(sf::Lines, 8);
		auto gridPos = world.toStratega(mousePos);
		auto gridPosI = SGA::Vector2i(std::floor(gridPos.x), std::floor(gridPos.y));
		selectedTile[0] = sf::Vertex(world.toSFML(gridPosI), sf::Color::Yellow);
		selectedTile[1] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y)), sf::Color::Yellow);

		selectedTile[2] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y)), sf::Color::Yellow);
		selectedTile[3] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y + 1)), sf::Color::Yellow);

		selectedTile[4] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y + 1)), sf::Color::Yellow);
		selectedTile[5] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x, gridPosI.y + 1)), sf::Color::Yellow);

		selectedTile[6] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x, gridPosI.y + 1)), sf::Color::Yellow);
		selectedTile[7] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x, gridPosI.y)), sf::Color::Yellow);

		window.draw(selectedTile);

		ImGui::Begin("Mouse window");
		std::string gridPosText = "Grid Position: (" + std::to_string(gridPos.x) + "," + std::to_string(gridPos.y) + ")";
		auto strategaMousPos = world.toStrategaRounded( mousePos);
		std::string gridMousePosText = "Grid Position Round: (" + std::to_string(strategaMousPos.x) + "," + std::to_string(strategaMousPos.y) + ")";
		std::string windowMousePosText = "Mouse Position Round: (" + std::to_string((int)mousePos.x) + "," + std::to_string((int)mousePos.y) + ")";
		//ImGui::Text(gridPosText.c_str());
		ImGui::Text(gridMousePosText.c_str());
		ImGui::Text(windowMousePosText.c_str());
		ImGui::End();

		sf::CircleShape s(10);
		s.setOrigin({ 5, 5 });
		s.setPosition(world.toSFML(SGA::Vector2f{ 0, 0 }));
		s.setFillColor(sf::Color::Red);
		window.draw(s);

		ImGui::SFML::Render(window);
		window.display();
	}

	void NewTBSGameRenderer::mouseScrolled(const sf::Event& event)
	{
		// Determine the scroll direction and adjust the zoom level
		if (event.mouseWheelScroll.delta <= -1)
			zoomValue = std::min(10.f, zoomValue + .1f);
		else if (event.mouseWheelScroll.delta >= 1)
			zoomValue = std::max(.5f, zoomValue - .1f);

		// Update our view
		sf::View view = window.getView();
		view.setSize(window.getDefaultView().getSize()); // Reset the size
		view.zoom(zoomValue); // Apply the zoom level (this transforms the view)
		window.setView(view);
	}
	void NewTBSGameRenderer::mouseButtonReleased(const sf::Event& event)
	{
		// Mouse button is released, no longer move
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			dragging = false;
			
		}
	}
	void NewTBSGameRenderer::mouseButtonPressed(const sf::Event& event)
	{
		// Mouse button is pressed, get the position and set moving as active
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			dragging = true;
			oldMousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
		}
		if (event.mouseButton.button == sf::Mouse::Right)
		{
			
		}
	}

	void NewTBSGameRenderer::mouseMoved(const sf::Event& event)
	{
		if (!dragging)
			return;

		// Determine how the cursor has moved
		auto newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
		auto deltaPos = oldMousePosition - newPos;

		// Move our view accordingly and update the window
		sf::View view = window.getView();
		view.setCenter(view.getCenter() + deltaPos);
		window.setView(view);

		// Save the new position as the old one
		// We're recalculating this, since we've changed the view
		oldMousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
	}

	void NewTBSGameRenderer::setPlayerPointOfView(int playerID)
	{
		
		update(state);
	}
}
