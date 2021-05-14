#include <Stratega/GUI/TBSGameRenderer.h>
#include <Stratega/GUI/GridUtils.h>
#include <Stratega/Configuration/RenderConfig.h>
#include <Stratega/Configuration/GameConfig.h>

#include <SFML/Window.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <sstream>

namespace SGA
{
	TBSGameRenderer::TBSGameRenderer()
		: config(nullptr),
		  state(),
		  fowState(),
		  selectedAction(),
		  window(sf::VideoMode(800, 600), "Stratega GUI", sf::Style::Default | sf::Style::Titlebar),
		  pointOfViewPlayerID(0),
		  fowSettings(),
		  zoomValue(5.f),
		  dragging(false)
	{
		fowSettings.selectedPlayerID = pointOfViewPlayerID;
		fowSettings.renderFogOfWar = true;
		fowSettings.renderType = FogRenderType::Fog;

		// Initialize View
		sf::View view = window.getView();
		view.setCenter(window.getSize().x / 2., window.getSize().y / 2.);
		view.setSize(window.getDefaultView().getSize()); // Reset the size
		view.zoom(zoomValue); // Apply the zoom level (this transforms the view)
		window.setView(view);
	}

	void TBSGameRenderer::init(const GameState& initialState, const GameConfig& gameConfig)
	{
		config = &gameConfig;
		
		// Load textures
		for (const auto& namePathPair : gameConfig.renderConfig->entitySpritePaths)
		{
			assetCache.loadTexture(namePathPair.first, namePathPair.second);
		}

		assetCache.loadTexture("selected", "./GUI/Assets/Tiles/selected.png");
		assetCache.loadFont("font", "./GUI/Assets/arial.ttf");

		tileMap.init(initialState, gameConfig, *gameConfig.renderConfig);
		entityRenderer.init(initialState, gameConfig, *gameConfig.renderConfig);

		ImGui::SFML::Init(window);

		update(initialState);
	}


	void TBSGameRenderer::update(const GameState& state)
	{
		this->state = state;
		updateFow();
		selectedAction.reset();

		// Update available actions
		actionsSettings.actionsHumanPlayer = config->forwardModel->generateActions(fowState, pointOfViewPlayerID);
	}

	void TBSGameRenderer::handleInput()
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered())
				continue;

			switch (event.type)
			{
				case sf::Event::Closed: {endGameRequested = true;; break; }
				case sf::Event::MouseWheelScrolled: { mouseScrolled(event); break; }
				case sf::Event::MouseButtonReleased: { mouseButtonReleased(event); break; }
				case sf::Event::MouseButtonPressed: { mouseButtonPressed(event); break; }
				case sf::Event::MouseMoved: { mouseMoved(event); 	break; }
				//case sf::Event::KeyPressed: {keyPressed(event); break;	}
				default:  break;
			}
		}
	}

	void TBSGameRenderer::render()
	{
		handleInput();
		
		window.clear();
		renderLayers();
		ImGui::SFML::Update(window, deltaClock.restart());
		createHUD();
		ImGui::SFML::Render(window);
		window.display();
	}

	void TBSGameRenderer::renderLayers()
	{
		tileMap.update(state, fowState, fowSettings.renderFogOfWar, fowSettings.renderType);
		window.draw(tileMap);

		//Add selected tileactionSettings.waitingForPosition
		auto currentMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		sf::Vector2i mouseGridPos = toGrid(currentMousePos);
		if (fowState.isInBounds(Vector2i(mouseGridPos.x, mouseGridPos.y)))
		{
			sf::Texture& texture = assetCache.getTexture("selected");
			sf::Vector2f origin(TILE_ORIGIN_X, TILE_ORIGIN_Y);
			sf::Sprite selectedTile(texture);

			selectedTile.setPosition(toISO(mouseGridPos.x, mouseGridPos.y));
			selectedTile.setOrigin(origin);
			window.draw(selectedTile);
		}

		//Draw entities
		entityRenderer.update(fowSettings.renderFogOfWar ? fowState : state);
		window.draw(entityRenderer);

		//Draw possible actions
		std::vector<sf::CircleShape> actionsShapes;
		if (actionsSettings.waitingForPosition)
		{
			for (auto& possibleAction : actionsSettings.actionsHumanPlayer)
			{
				//Check if action is compatible with the selected type and targets
				if (possibleAction.actionTypeID == -1 || possibleAction.actionTypeFlags == ActionFlag::ContinuousAction
					|| possibleAction.actionTypeFlags == ActionFlag::AbortContinuousAction
					|| possibleAction.actionTypeID != actionsSettings.actionTypeSelected)
					continue;

				//Get source
				const auto& actionType = state.gameInfo->getActionTypeConst(possibleAction.actionTypeID);
				//Check the source and the selected entity is the same
				if (actionType.sourceType == ActionSourceType::Entity)
				{
					auto& entity = *possibleAction.targets[0].getEntity(state);
					if (entity.id != *actionsSettings.selectedEntities.begin())
						continue;
				}

				for (auto& actionTarget : possibleAction.targets)
				{
					if (actionTarget.getType() == ActionTarget::Position)
					{
						auto position = actionTarget.getPosition(state);

						sf::CircleShape possibleActionPositionShape(15);
						possibleActionPositionShape.setFillColor(sf::Color::White);

						sf::Vector2f temp = toISO(position.x, position.y);
						possibleActionPositionShape.setPosition(temp + sf::Vector2f(TILE_OFFSET_ORIGIN_X, TILE_OFFSET_ORIGIN_Y));

						actionsShapes.emplace_back(possibleActionPositionShape);
					}
				}

				for (const auto& shape : actionsShapes)
				{
					window.draw(shape);
				}
			}
		}
		else if (actionsSettings.waitingForEntity)
		{
			for (auto& possibleAction : actionsSettings.actionsHumanPlayer)
			{
				//Check if action is compatible with the selected type and targets
				if (possibleAction.actionTypeID == -1 || possibleAction.actionTypeFlags == ActionFlag::ContinuousAction
					|| possibleAction.actionTypeFlags == ActionFlag::AbortContinuousAction ||
					possibleAction.actionTypeID != actionsSettings.actionTypeSelected)
					continue;

				//Get source
				const auto& actionType = state.gameInfo->getActionTypeConst(possibleAction.actionTypeID);

				//Check the source and the selected entity is the same
				if (actionType.sourceType == ActionSourceType::Entity)
				{
					auto& entity = *possibleAction.targets[0].getEntity(state);
					if (entity.id != *actionsSettings.selectedEntities.begin())
						continue;
				}

				//Avoid source entity
				for (int i = 1; i < possibleAction.targets.size(); ++i)
				{
					if (possibleAction.targets[i].getType() == ActionTarget::EntityReference)
					{
						auto position = possibleAction.targets[i].getPosition(state);

						sf::CircleShape possibleActionPositionShape(15);
						possibleActionPositionShape.setFillColor(sf::Color::White);

						sf::Vector2f temp = toISO(position.x, position.y);
						possibleActionPositionShape.setPosition(temp + sf::Vector2f(TILE_OFFSET_ORIGIN_X, TILE_OFFSET_ORIGIN_Y));

						actionsShapes.emplace_back(possibleActionPositionShape);
					}
				}

				for (const auto& shape : actionsShapes)
				{
					window.draw(shape);
				}
			}
		}
	}

	void TBSGameRenderer::mouseScrolled(const sf::Event& event)
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
	void TBSGameRenderer::mouseButtonReleased(const sf::Event& event)
	{
		// Mouse button is released, no longer move
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			dragging = false;
		}
	}
	void TBSGameRenderer::mouseButtonPressed(const sf::Event& event)
	{
		// Mouse button is pressed, get the position and set moving as active
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			sf::Vector2i pos = toGrid(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
			//If selected unit we check if there is action in tile
			if (fowSettings.renderFogOfWar && (pointOfViewPlayerID == fowSettings.selectedPlayerID || !fowSettings.renderFogOfWar))
			{
				if (actionsSettings.waitingForPosition)
				{
					assignPosition(state, actionsSettings, { (float)pos.x,(float)pos.y });
				}
				else
				{
					if (!actionsSettings.waitingForEntity)
					{
						actionsSettings.selectedEntities.clear();
						actionsSettings.actionTypeSelected = -1;
					}
				}
			}

			auto* selectedEntity = const_cast<GameState&>(state).getEntity(Vector2f(pos.x, pos.y));
			if (selectedEntity && ((fowSettings.renderFogOfWar && (pointOfViewPlayerID == fowSettings.selectedPlayerID)) || !fowSettings.renderFogOfWar))
			{
				//Assign selected unit
				if (actionsSettings.waitingForEntity)
				{
					assignEntity(state, actionsSettings, selectedEntity->id);
				}
				else
				{
					//actionsSettings.selectedEntities.clear();

					//Pick up entity
					if (selectedEntity->ownerID == pointOfViewPlayerID)
						actionsSettings.selectedEntities.emplace(selectedEntity->id);
				}
			}
			else
			{
				//Restart selected actions of unit and selected unit
				dragging = true;
				oldMousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
			}
		}
		if (event.mouseButton.button == sf::Mouse::Right)
		{
			selectedAction = Action::createEndAction(pointOfViewPlayerID);
		}
	}

	void TBSGameRenderer::mouseMoved(const sf::Event& event)
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

	void TBSGameRenderer::createHUD()
	{
		createWindowInfo();
		createWindowUnits();
		createWindowActions();
		createWindowPlayerParameters();
		createEntityInformation();
		createWindowFogOfWar();
		createActionBar();
	}

	void TBSGameRenderer::createWindowInfo()
	{
		ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);

		ImGui::Begin("Info window");
		std::string text = "Tick:" + std::to_string(state.currentTick);
		ImGui::Text(text.c_str());
		text = "Current Player : " + std::to_string(state.currentPlayer);
		ImGui::Text(text.c_str());
		ImGui::End();
	}

	void TBSGameRenderer::createWindowFogOfWar()
	{
		ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);

		ImGui::Begin("Fog of War window");
		if(fowController(state, fowSettings))
		{
			updateFow();
		}
		ImGui::End();
	}

	void TBSGameRenderer::createEntityInformation()
	{
		if (!actionsSettings.selectedEntities.empty())
		{
			ImGuiWindowFlags window_flags = 0;
			//window_flags += ImGuiWindowFlags_NoTitleBar;
			//window_flags += ImGuiWindowFlags_NoScrollbar;
			//window_flags += ImGuiWindowFlags_MenuBar;
			//window_flags+= ImGuiWindowFlags_NoMove;
			window_flags += ImGuiWindowFlags_NoResize;
			window_flags += ImGuiWindowFlags_NoCollapse;
			window_flags += ImGuiWindowFlags_NoNav;
			//window_flags+= ImGuiWindowFlags_NoBackground;
			window_flags += ImGuiWindowFlags_NoBringToFrontOnFocus;

			ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Always);
			ImGui::SetNextWindowPos(ImVec2(0, static_cast<float>(window.getSize().y)), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
			ImGui::Begin("Entity Information", NULL, window_flags);

			auto* selectedEntity = state.getEntityConst(*actionsSettings.selectedEntities.begin());
			auto entityType = state.gameInfo->getEntityType(selectedEntity->typeID);

			ImGui::Text(entityType.name.c_str());
			ImGui::Columns(2, "mixed");
			ImGui::SetColumnWidth(0, 100.0f);

			ImGui::Separator();

			//Add units
			sf::Texture& texture = assetCache.getTexture(entityType.name);

			ImGui::Image(texture, ImVec2(100, 100), sf::Color::White, sf::Color::Transparent);


			ImGui::NextColumn();
			ImGui::Text("Parameters: ");

			int parameterIndex = 0;
			for (const auto& parameter : entityType.parameters)
			{
				//Double to string with 2 precision
				std::stringstream stream;
				stream << std::fixed << std::setprecision(2) << selectedEntity->parameters[parameterIndex++];
				std::string valueParameter = stream.str();

				std::string parameterInfo = parameter.second.name + ": " + valueParameter;
				ImGui::BulletText(parameterInfo.c_str());
			}

			ImGui::NextColumn();

			ImGui::Columns(1);
			ImGui::Separator();

			ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;
			ImGui::BeginChild("help", ImVec2(0, 80), true, child_flags);


			for (auto& entity : state.getPlayerEntities(fowSettings.selectedPlayerID))
			{
				//Check if entity have sprite
				auto entityType = state.gameInfo->getEntityType(entity->typeID);
				//Add units
				sf::Texture& texture = assetCache.getTexture(entityType.name);

				if (ImGui::ImageButton(texture, ImVec2(50, 50), -10))
				{
					/*selectedEntityID = entity->id;*/
				}
				ImGui::SameLine();
			}

			ImGui::EndChild();
			ImGui::SameLine();

			ImGui::Spacing();
			ImGui::End();
		}

	}

	void TBSGameRenderer::createActionBar()
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags += ImGuiWindowFlags_NoTitleBar;
		window_flags += ImGuiWindowFlags_NoScrollbar;
		//window_flags += ImGuiWindowFlags_MenuBar;
		//window_flags+= ImGuiWindowFlags_NoMove;
		//window_flags += ImGuiWindowFlags_NoResize;
		window_flags += ImGuiWindowFlags_NoCollapse;
		window_flags += ImGuiWindowFlags_NoNav;
		//window_flags+= ImGuiWindowFlags_NoBackground;
		window_flags += ImGuiWindowFlags_NoBringToFrontOnFocus;


		// We specify a default position/size in case there's no data in the .ini file.
		// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
		ImGui::SetNextWindowPos(ImVec2(window.getSize().x / 2.f, window.getSize().y / 1.1f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(0, 0));

		ImGui::Begin("Bottom Bar", NULL, window_flags);

		ImGui::Separator();

		ImGui::Text("Actions");

		//Ask widget to get
		auto actionsToExecute = getWidgetResult(const_cast<GameState&>(state), actionsSettings, pointOfViewPlayerID);
		if (!actionsToExecute.empty())
		{
			selectedAction = actionsToExecute.front();
		}

		ImGui::Separator();
		ImGui::End();
	}

	void TBSGameRenderer::createWindowUnits()
	{
		ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 150), ImGuiCond_FirstUseEver);
		ImGui::Begin("Entities");

		ImGui::BeginChild("Scrolling");

		for (auto& unit : state.entities)
		{
			auto& type = state.gameInfo->getEntityType(unit.typeID);
			std::string unitInfo;
			unitInfo = type.name + " " + std::to_string(unit.id) + " PID: " + std::to_string(unit.ownerID);
			ImGui::Text(unitInfo.c_str());
		}

		ImGui::EndChild();
		ImGui::End();
	}

	void TBSGameRenderer::createWindowActions()
	{
		ImGui::SetNextWindowSize(ImVec2(200, 150), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin("Actions");
		ImGui::BeginChild("Scrolling");
		ImGui::BeginGroup();

		int index = 0;
		for (auto action : actionsSettings.actionsHumanPlayer)
		{

			std::string actionInfo = std::to_string(index);
			if (action.actionTypeID == -1)
			{
				if (action.actionTypeFlags == ActionFlag::AbortContinuousAction)
				{
					if (action.targets[0].getType() == ActionTarget::EntityReference)
					{
						//We need to find the continues action name that will abort
						auto& sourceEntity = *state.getEntityConst(action.targets[0].getEntityID());
						for (auto& continueAction : sourceEntity.continuousAction)
						{
							if (continueAction.continuousActionID == action.continuousActionID)
							{
								const ActionType& actionType = state.gameInfo->getActionType(continueAction.actionTypeID);
								actionInfo += " Abort " + actionType.name;
							}
						}
					}
					else
					{
						//We need to find the continues action name that will abort
						auto& sourcePlayer = action.targets[0].getPlayer(const_cast<GameState&>(state));
						for (auto& continueAction : sourcePlayer.continuousAction)
						{
							if (continueAction.continuousActionID == action.continuousActionID)
							{
								const ActionType& actionType = state.gameInfo->getActionType(continueAction.actionTypeID);
								actionInfo += " Abort " + actionType.name;
							}
						}
					}



				}
				else
					actionInfo += " SpecialAction";
			}
			else
			{
				const ActionType& actionType = state.gameInfo->getActionType(action.actionTypeID);

				actionInfo += " " + actionType.name;

				//TODO Clean this :D IS TEMPORAL
				for (auto& targetType : action.targets)
				{
					switch (targetType.getType())
					{
					case ActionTarget::Position:
						actionInfo += " x:" + std::to_string((int)targetType.getPosition(state).x) + ",y:" + std::to_string((int)targetType.getPosition(state).y);
						break;
					case ActionTarget::EntityReference:
						actionInfo += state.gameInfo->getEntityType(state.getEntityConst(targetType.getEntityID())->typeID).name;
						break;
					case ActionTarget::PlayerReference:
						actionInfo += " Player: " + std::to_string(pointOfViewPlayerID);
						break;
					case ActionTarget::TechnologyReference:
						actionInfo += " Technology: " + state.gameInfo->technologyTreeCollection->getTechnology(targetType.getTechnologyID()).name;
						break;
					case ActionTarget::EntityTypeReference:
						actionInfo += " Entity: " + targetType.getEntityType(state).name;
						break;
					case ActionTarget::ContinuousActionReference:
						break;
					}
				}

			}

			index++;

			if (ImGui::Button(actionInfo.c_str()))
			{
				selectedAction = action;
				break;
			}
		}

		ImGui::EndGroup();

		ImGui::EndChild();
		ImGui::End();
	}

	void TBSGameRenderer::createWindowPlayerParameters()
	{
		ImGui::SetNextWindowSize(ImVec2(100, 150), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_FirstUseEver);
		ImGui::Begin("PlayerParameters");
		ImGui::BeginChild("Scrolling");
		ImGui::BeginGroup();

		const auto* player = state.getPlayer(fowSettings.selectedPlayerID);
		for (const auto& parameter : *state.gameInfo->playerParameterTypes)
		{
			//Double to string with 2 precision
			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << player->parameters[parameter.second.index];
			std::string valueParameter = stream.str();

			std::string parameterInfo = parameter.second.name + ": " + valueParameter;
			ImGui::BulletText(parameterInfo.c_str());
		}

		ImGui::EndGroup();

		ImGui::EndChild();
		ImGui::End();
	}

	void TBSGameRenderer::setPlayerPointOfView(int playerID)
	{
		pointOfViewPlayerID = playerID;
		fowSettings.selectedPlayerID = playerID;
		update(state);
	}

	ActionAssignment TBSGameRenderer::getPlayerActions()
	{
		ActionAssignment returnValue;
		if(isActionAvailable())
		{
			returnValue.assignActionOrReplace(selectedAction.value());
		}
		return returnValue;
	}

	bool TBSGameRenderer::isActionAvailable() const
	{
		return selectedAction.has_value();
	}

	void TBSGameRenderer::updateFow()
	{
		fowState = state;
		fowState.applyFogOfWar(fowSettings.selectedPlayerID);
	}

	bool TBSGameRenderer::isGameEndRequested()
	{
		return endGameRequested;
	}
}
