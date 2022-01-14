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
#include <Stratega/NewGUI/MouseInformationWidget.h>
#include <Stratega/NewGUI/GameStateInformationWidget.h>
#include <Stratega/NewGUI/PlayerInformationWidget.h>
#include <Stratega/NewGUI/TBSActionsWidget.h>
#include <Stratega/NewGUI/WorldControllerWidget.h>

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
		dragging(false),
		selectedAction()
	{
		//Initialize View
		sf::View view = window.getView();
		view.setCenter(static_cast<float>(window.getSize().x / 2.), static_cast<float>(window.getSize().y / 2.));
		view.setSize(window.getDefaultView().getSize()); // Reset the size
		view.zoom(zoomValue); // Apply the zoom level (this transforms the view)
		window.setView(view);

		
	}

	void NewTBSGameRenderer::init(const GameState& initialState, const GameConfig& gameConfig)
	{
		config = &gameConfig;

		world = World::createIsometricGrid(256, 144, { initialState.getBoardWidth(), initialState.getBoardHeight() });
		resourceManager = ResourceManager::constructFromConfig(gameConfig);
		renderTarget = std::make_unique<SGARenderTarget>(window, *resourceManager, world, *gameConfig.renderConfig);
		ImGui::SFML::Init(window);

		update(initialState);

		//Update world
		world.init(initialState, *gameConfig.renderConfig);

		//Add layout widget
		widgets.emplace_back(std::make_unique<GridLayoutWidget>("Grid Layout", window, world, config->forwardModel.get()));
		widgets.emplace_back(std::make_unique<MouseInformationWidget>("Mouse Information", window, world, config->forwardModel.get()));
		widgets.emplace_back(std::make_unique<WorldControllerWidget>("World Controller", window, world, config->forwardModel.get()));
		widgets.emplace_back(std::make_unique<GridInformationWidget>("Grid Information", window, world, config->forwardModel.get()));
		widgets.emplace_back(std::make_unique<TBSActionsWidget>("Actions Controller", window, world, config->forwardModel.get(), temp));
		widgets.emplace_back(std::make_unique<GameStateInformationWidget>("State Information", window, world, config->forwardModel.get()));
		widgets.emplace_back(std::make_unique<PlayerInformationWidget>("Player Information", window, world, config->forwardModel.get()));
	}

	ActionAssignment NewTBSGameRenderer::getPlayerActions()
	{
		ActionAssignment returnValue= temp;
		temp.clear();
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
			if(widget->enabled)
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
		auto dt = deltaClock.restart();
		world.render(*renderTarget, dt.asSeconds());

		ImGui::SFML::Update(window, dt);
		
		//Render widgets
		for (auto& widget : widgets)
		{
			if (widget->enabled)
				widget->render(*renderTarget);
		}

		//Components window
		ImGui::Begin("Widgets controller");
		for (auto& widget : widgets)
		{
			ImGui::Checkbox(widget->name.c_str(), &widget->enabled);
		}
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

		for (auto& widget : widgets)
		{
			if (widget->enabled)
				widget->mouseScrolled(event);
		}
	}
	void NewTBSGameRenderer::mouseButtonReleased(const sf::Event& event)
	{
		// Mouse button is released, no longer move
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			dragging = false;
			
		}

		for (auto& widget : widgets)
		{
			if (widget->enabled)
				widget->mouseButtonReleased(event);
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

		for (auto& widget : widgets)
		{
			if (widget->enabled)
				widget->mouseButtonPressed(event);
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

		for (auto& widget : widgets)
		{
			if (widget->enabled)
				widget->mouseMoved(event);
		}
	}

	void NewTBSGameRenderer::setPlayerPointOfView(int playerID)
	{
		update(state);
		playerID = playerID;
	}
}
