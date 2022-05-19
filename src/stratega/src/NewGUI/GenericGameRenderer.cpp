#include <Stratega/NewGUI/GenericGameRenderer.h>
#include <Stratega/NewGUI/ResourceManager.h>
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
#include <Stratega/NewGUI/ActionsWidget.h>
#include <Stratega/NewGUI/WorldControllerWidget.h>

DISABLE_WARNING_PUSH
#if defined(__clang__)    
DISABLE_WARNING_FORMAT
#elif defined(__GNUC__)
DISABLE_WARNING_FORMAT
#endif

namespace SGA
{
	GenericGameRenderer::GenericGameRenderer(SGA::Vector2i& resolution)
		: config(nullptr),
		state(),
		fowState(),
		window(sf::VideoMode(static_cast<unsigned>(resolution.x), static_cast<unsigned>(resolution.y)), "Stratega GUI", sf::Style::Default | sf::Style::Titlebar),
		zoomValue(5.f),
		dragging(false),
		playerID(-1)
	{
		//Initialize View
		sf::View view = window.getView();
		view.setCenter(static_cast<float>(window.getSize().x / 2.), static_cast<float>(window.getSize().y / 2.));
		view.setSize(window.getDefaultView().getSize()); // Reset the size
		view.zoom(zoomValue); // Apply the zoom level (this transforms the view)
		window.setView(view);

		settings.renderFogOfWar = true;
		settings.renderType = FogRenderType::Fog;
	}

	void GenericGameRenderer::init(const GameState& initialState, const GameConfig& gameConfig)
	{
		config = &gameConfig;
		resourceManager = ResourceManager::constructFromConfig(gameConfig);
		if(config->renderConfig->isIsometricGrid) 
			world = World::createIsometricGrid(static_cast<int>(config->renderConfig->tileSpriteSize.x), static_cast<int>(config->renderConfig->tileSpriteSize.y), { initialState.getBoardWidth(), initialState.getBoardHeight() }, selectedEntities, settings);
		else
			world = World::createRectangleGrid(static_cast<int>(config->renderConfig->tileSpriteSize.x), static_cast<int>(config->renderConfig->tileSpriteSize.y), { initialState.getBoardWidth(), initialState.getBoardHeight() }, selectedEntities, settings);

		
		renderTarget = std::make_unique<SGARenderTarget>(window, *resourceManager, world, *gameConfig.renderConfig);
		ImGui::SFML::Init(window);
		settings.renderFogOfWar = config->applyFogOfWar;
		state = initialState;
		updateFow();

		//Update world
		world.init(initialState,fowState ,*gameConfig.renderConfig);

		//Add layout widget
		widgets.emplace_back(std::make_unique<GridLayoutWidget>("Grid Layout", window, world, config->forwardModel.get()));
		widgets.emplace_back(std::make_unique<MouseInformationWidget>("Mouse Information", window, world, config->forwardModel.get()));
		widgets.emplace_back(std::make_unique<WorldControllerWidget>("World Controller", window, world, config->forwardModel.get()));
		widgets.emplace_back(std::make_unique<GridInformationWidget>("Grid Information", window, world, config->forwardModel.get(), selectedEntities));
		widgets.emplace_back(std::make_unique<ActionsWidget>("Actions Controller", window, world, config->forwardModel.get(), temp, futureActionsToPlay, selectedEntities, playerID));
		widgets.emplace_back(std::make_unique<GameStateInformationWidget>("State Information", window, world, config->forwardModel.get()));
		widgets.emplace_back(std::make_unique<PlayerInformationWidget>("Player Information", window, world, config->forwardModel.get()));
		widgets.emplace_back(std::make_unique<FOWControllerWidget>("FOW Controller", window, world, config->forwardModel.get(), &settings));
	}

	ActionAssignment GenericGameRenderer::getPlayerActions()
	{
		ActionAssignment returnValue= temp;
		temp.clear();
		return returnValue;
	}

	void GenericGameRenderer::update(const GameState& newState)
	{
		state = newState;
		updateFow();

		//Update world
		world.update(state,fowState);

		//Render widgets
		for (auto& widget : widgets)
		{
			if(widget->enabled)
				widget->update(fowState);
		}

		if (state.getGameType() == GameType::RTS)
		{
			auto& entityActions = futureActionsToPlay.getEntityActionsNonConst();

			auto it = entityActions.begin();
			//Assign entity actions if they are still valid
			while (it != entityActions.end())
			{
				if (!it->second.validate(fowState)) {

					it = entityActions.erase(it);
				}
				else
				{
					temp.assignActionOrReplace(it->second);
					++it;
				}
			}
			
			auto& playerActions = futureActionsToPlay.getPlayerActionsNonConst();

			auto it2 = playerActions.begin();
			//Assign entity actions if they are still valid
			while (it2 != playerActions.end())
			{
				if (!it2->second.validate(fowState)) {

					it2 = playerActions.erase(it2);
				}
				else
				{
					temp.assignActionOrReplace(it2->second);
					++it2;
				}
			}
		}
	}

	void GenericGameRenderer::updateFow()
	{
		fowState = state;
		if(settings.renderFogOfWar)
			fowState.applyFogOfWar(settings.selectedPlayerID);
	}

	void GenericGameRenderer::handleInput()
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
			case sf::Event::Closed: {endGameRequested = true; break; }
			case sf::Event::MouseWheelScrolled: { mouseScrolled(event); break; }
			case sf::Event::MouseButtonReleased: { mouseButtonReleased(event); break; }
			case sf::Event::MouseButtonPressed: { mouseButtonPressed(event); break; }
			case sf::Event::MouseMoved: { mouseMoved(event); 	break; }
			case sf::Event::KeyPressed: {keyPressed(event); break;	}
			default:  break;
			}
		}
	}

	bool GenericGameRenderer::isGameEndRequested()
	{
		return endGameRequested;
	}

	void GenericGameRenderer::render()
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

		if (settings.settingsChanged)
		{
			updateFow();
			//Update world
			world.update(state ,fowState);
			settings.settingsChanged = false;

			//Render widgets
			for (auto& widget : widgets)
			{
				if (widget->enabled)
					widget->update(fowState);
			}
		}

		//Components window
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 10), ImGuiCond_FirstUseEver);
		ImGui::Begin("Widgets controller");
		for (auto& widget : widgets)
		{
			ImGui::Checkbox(widget->name.c_str(), &widget->enabled);
		}
		ImGui::End();

		ImGui::SFML::Render(window);
		window.display();
	}

	void GenericGameRenderer::mouseScrolled(const sf::Event& event)
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
	void GenericGameRenderer::keyPressed(const sf::Event& /*event*/)
	{
		// Camera movement
		sf::Vector2f movementDir;
		float cameraSpeed = 50;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			movementDir.y = -cameraSpeed;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			movementDir.y = cameraSpeed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			movementDir.x = -cameraSpeed;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			movementDir.x = cameraSpeed;
		}
		sf::View view = window.getView();
		view.setCenter(view.getCenter() + movementDir);
		window.setView(view);

	}
	void GenericGameRenderer::mouseButtonReleased(const sf::Event& event)
	{
		if (state.getGameType() == GameType::TBS)
		{
			// Mouse button is released, no longer move
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				dragging = false;
			}
		}
		else
		{
			// Mouse button is released, no longer move
			if (event.mouseButton.button == sf::Mouse::Middle) {
				dragging = false;
			}

			//if (event.mouseButton.button == sf::Mouse::Left)
			//{
			//	dragging = false;

			//	sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			//	auto worldPos = toGrid(pos);
			//	auto movementDir = pos - oldMousePosition;
			//	auto movementDistance = std::sqrt(movementDir.x * movementDir.x + movementDir.y * movementDir.y);

			//	if (movementDistance <= 10.0)
			//	{
			//		// The user clicked somewhere
			//		if (!actionsSettings.selectedEntities.empty())
			//		{
			//			auto* unit = state.getEntity(SGA::Vector2f(worldPos.x, worldPos.y), 0.5);

			//			if (unit)
			//			{
			//				//we click on someone
			//				if (actionsSettings.waitingForEntity)
			//				{
			//					assignEntity(state, actionsSettings, unit->getID());
			//				}
			//				else
			//				{
			//					actionsSettings.selectedEntities.clear();
			//				}

			//			}
			//			else
			//			{
			//				if (actionsSettings.waitingForPosition)
			//				{
			//					auto gridPos = toGridFloat(pos);
			//					assignPosition(state, actionsSettings, { gridPos.x,gridPos.y });
			//				}
			//				else
			//				{
			//					actionsSettings.selectedEntities.clear();
			//				}
			//			}
			//		}
			//		else
			//		{
			//			if (actionsSettings.waitingForPosition)
			//			{
			//				auto gridPos = toGrid(pos);
			//				assignPosition(state, actionsSettings, { static_cast<float>(gridPos.x),static_cast<float>(gridPos.y) });
			//			}
			//			else
			//			{
			//				actionsSettings.selectedEntities.clear();
			//				actionsSettings.reset();
			//			}
			//		}
			//	}
			//	else
			//	{
			//		// The user selected an area
			//		auto prevMouseWorldPos = oldMousePosition;
			//		auto xLeft = std::min(prevMouseWorldPos.x, pos.x);
			//		auto xRight = std::max(prevMouseWorldPos.x, pos.x);
			//		auto yLeft = std::min(prevMouseWorldPos.y, pos.y);
			//		auto yRight = std::max(prevMouseWorldPos.y, pos.y);

			//		for (auto& unit : state.getEntities())
			//		{
			//			sf::Vector2f screenPos = toISO(unit.x(), unit.y());
			//			if (screenPos.x > xLeft && screenPos.x < xRight && screenPos.y > yLeft && screenPos.y < yRight)
			//			{
			//				if (unit.getOwnerID() == pointOfViewPlayerID)
			//					actionsSettings.selectedEntities.emplace(unit.getID());
			//			}
			//		}
			//	}
			//}
		}
		

		for (auto& widget : widgets)
		{
			if (widget->enabled)
				widget->mouseButtonReleased(event);
		}
	}
	void GenericGameRenderer::mouseButtonPressed(const sf::Event& event)
	{
		// Mouse button is pressed, get the position and set moving as active
		if (state.getGameType() == GameType::TBS)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				dragging = true;
				oldMousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
			}
		}
		else
		{
			// Mouse button is pressed, get the position and set moving as active
			if (event.mouseButton.button == sf::Mouse::Middle)
			{
				dragging = true;
				oldMousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
			}
		}

		for (auto& widget : widgets)
		{
			if (widget->enabled)
				widget->mouseButtonPressed(event);
		}
	}
	void GenericGameRenderer::mouseMoved(const sf::Event& event)
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

	void GenericGameRenderer::setPlayerPointOfView(int newPlayerID)
	{
		this->playerID = newPlayerID;
		settings.selectedPlayerID = newPlayerID;
		update(state);		
	}
}
