#include <TBSGameStateRender.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iomanip>
#include <sstream>
#include <GridUtils.h>

#include <Stratega/Configuration/GameConfig.h>
#include <Configuration/RenderConfig.h>
#include <Widgets/FogOfWarController.h>

namespace SGA
{
	TBSGameStateRender::TBSGameStateRender(TBSGame& game, const GameConfig& gameConfig, const RenderConfig& renderConfig, int playerID) :
		GameStateRenderer{ playerID},
		game(&game),
		gameStateCopy(game.getState()),
		gameStatesBuffer(50)
	{
		init(gameConfig, renderConfig);
		
		if (gameStateCopy.currentPlayer == getPlayerID())
		{
			std::cout << "Wait for GUI Action" << std::endl;
			waitForHumanToPlay();
		}

		//Initialize gameStateFog and apply fog to it		
		gameStateCopyFogOfWar = gameStateCopy;
		gameStateCopyFogOfWar.applyFogOfWar(fowSettings.selectedPlayerID);
	}

	void TBSGameStateRender::init()
	{
		GameStateRenderer::init();
	}

	void TBSGameStateRender::onGameStateAdvanced()
	{
		std::lock_guard<std::mutex> lockGuard(mutexRender);
		gameStateCopy = game->getStateCopy();
		gameStateCopyFogOfWar = game->getStateCopy();
		gameStateCopyFogOfWar.applyFogOfWar(fowSettings.selectedPlayerID);

		//Add state to buffer
		gameStatesBuffer.add(gameStateCopy);
		gameStatesBufferRCurrentIndex = gameStatesBuffer.getFront();

		if (gameStateCopy.currentPlayer == getPlayerID())
		{
			std::cout << "Wait for GUI Action" << std::endl;
			waitForHumanToPlay();
		}
	}

	void TBSGameStateRender::init(const GameConfig& gameConfig, const RenderConfig& renderConfig)
	{
		//Need to activate the context before adding new textures
		ctx.setActive(true);

		entityRenderer.init(gameStateCopy, gameConfig, renderConfig);
		tileMap.init(gameStateCopy, gameConfig, renderConfig);

		// Load textures
		for (const auto& namePathPair : renderConfig.entitySpritePaths)
		{
			assetCache.loadTexture(namePathPair.first, namePathPair.second);
		}
		
		// TODO Depends on location of configuration file, how to prevent that?
		assetCache.loadTexture("selected", "../../GUI/Assets/Tiles/selected.png");
		assetCache.loadFont("font", "../../GUI/Assets/arial.ttf");
	}

	void TBSGameStateRender::initializeView(sf::RenderWindow& window) const
	{
		//Apply zoom and center the view
		sf::View view = window.getView();
		view.setCenter((float)(window.getSize().x / 2), (float)(window.getSize().y / 2));
		view.setSize(window.getDefaultView().getSize()); // Reset the size	
		view.zoom(zoomValue); // Apply the zoom level (this transforms the view)
		window.setView(view);

		//Limit FPS window
		window.setFramerateLimit(fpsLimit);
	}

	void TBSGameStateRender::render()
	{
		sf::RenderWindow window(sf::VideoMode(800, 600), "Strategy Game Engine", sf::Style::Default | sf::Style::Titlebar);
		window.setActive(true);

		ImGui::SFML::Init(window);

		initializeView(window);

		//FPS stuff
		m_fps = 0;
		int m_fpscount = 0;
		sf::Clock clock;

		while (window.isOpen() && isRendering)
		{
			m_fpscount++;

			// one second elapsed? (= 1000 milliseconds)
			if (clock.getElapsedTime().asMilliseconds() > 1000)
			{
				// save the current counter value to m_fps
				m_fps = m_fpscount;

				// reset the counter and the interval
				m_fpscount = 0;
				clock.restart();
			}
			window.setTitle("fps:" + std::to_string(m_fps));

			//Artificial scope for the lock guard
			{
				std::lock_guard<std::mutex> lockGuard(mutexRender);
				handleInput(window);

				window.clear();

				drawLayers(window);
				ImGui::SFML::Update(window, deltaClock.restart());

				createHUD(window);
			}

			ImGui::SFML::Render(window);
			window.display();
		}
		game->end();
		window.close();
	}

	void TBSGameStateRender::handleInput(sf::RenderWindow& window)
	{
		//Get current View to edit it and apply changes
		sf::View view = window.getView();

		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered())
				return;

			switch (event.type)
			{
			case sf::Event::Closed: {windowClosed(event, view, window); break; }
			case sf::Event::MouseWheelScrolled: { mouseScrolled(event, view, window); break; }
			case sf::Event::MouseButtonReleased: { mouseButtonReleased(event, view, window); break; }
			case sf::Event::MouseButtonPressed: { mouseButtonPressed(event, view, window); break; }
			case sf::Event::MouseMoved: {/* Ignore mouse movement unless a button is pressed*/ mouseMoved(event, view, window); 	break; }
			case sf::Event::KeyPressed: {keyPressed(event, view, window); break;	}
			default:  break;
			}

		}
	}

	void TBSGameStateRender::windowClosed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)const
	{
		window.close();
		game->end();
	}
	void TBSGameStateRender::mouseScrolled(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
	{
		// Determine the scroll direction and adjust the zoom level
		if (event.mouseWheelScroll.delta <= -1)
			zoomValue = std::min(10.f, zoomValue + .1f);
		else if (event.mouseWheelScroll.delta >= 1)
			zoomValue = std::max(.5f, zoomValue - .1f);

		// Update our view
		view.setSize(window.getDefaultView().getSize()); // Reset the size
		view.zoom(zoomValue); // Apply the zoom level (this transforms the view)

		window.setView(view);
	}
	void TBSGameStateRender::mouseButtonReleased(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
	{
		// Mouse button is released, no longer move
		if (event.mouseButton.button == sf::Mouse::Left) {
			moving = false;
		}
	}
	void TBSGameStateRender::mouseButtonPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
	{
		// Mouse button is pressed, get the position and set moving as active
		if (event.mouseButton.button == sf::Mouse::Left)
		{

			sf::Vector2i pos = toGrid(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));

			//If selected unit we check if there is action in tile
			if (((fowSettings.renderFogOfWar && (getPlayerID() == fowSettings.selectedPlayerID)) || !fowSettings.renderFogOfWar))
			{
				if (actionsSettings.waitingForPosition)
				{
					assignPosition(gameStateCopy, actionsSettings, {(float) pos.x,(float)pos.y });
				}
				else
				{
					if(!actionsSettings.waitingForEntity)
					{
						actionsSettings.selectedEntities.clear();
						actionsSettings.actionTypeSelected = -1;
					}					
				}
			}

			Entity* selectedEntity = gameStateCopy.getEntity(Vector2f(pos.x, pos.y));

			if (selectedEntity && ((fowSettings.renderFogOfWar && (getPlayerID() == fowSettings.selectedPlayerID)) || !fowSettings.renderFogOfWar))
			{
				//Assign selected unit
				if (actionsSettings.waitingForEntity)
				{
					assignEntity(gameStateCopy, actionsSettings, selectedEntity->id);
				}
				else
				{
					//actionsSettings.selectedEntities.clear();

					//Pick up entity
					if (selectedEntity->ownerID == getPlayerID())
						actionsSettings.selectedEntities.emplace(selectedEntity->id);
				}
			}
			else
			{
				//Restart selected actions of unit and selected unit
				moving = true;
				oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
			}
		}
		if (event.mouseButton.button == sf::Mouse::Right)
		{
			if (waitForAction)
				playAction(Action::createEndAction(getPlayerID()));
		}
	}
	
	void TBSGameStateRender::mouseMoved(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
	{

		// Determine the new position in world coordinates
		const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

		//Set current mouse pos
		currentMousePos.x = newPos.x;
		currentMousePos.y = newPos.y;

		if (!moving)
			return;
		// Determine how the cursor has moved
		// Swap these to invert the movement direction
		const sf::Vector2f deltaPos = oldPos - newPos;

		// Move our view accordingly and update the window
		view.setCenter(view.getCenter() + deltaPos);
		window.setView(view);

		// Save the new position as the old one
		// We're recalculating this, since we've changed the view
		oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
	}
	
	void TBSGameStateRender::keyPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
	{
		if (event.key.code == sf::Keyboard::Left && drawGameStateBuffer)
		{
			std::cout << "Back in buffer" << std::endl;

			if (gameStatesBufferRCurrentIndex - 1 < 0)
				gameStatesBufferRCurrentIndex = gameStatesBuffer.getSize() - 1;
			else
				gameStatesBufferRCurrentIndex--;

			std::cout << std::to_string(gameStatesBuffer.getRelativePosition(gameStatesBufferRCurrentIndex)) << "/" << std::to_string(gameStatesBuffer.getSize()) << std::endl;
		}
		if (event.key.code == sf::Keyboard::Right && drawGameStateBuffer)
		{
			std::cout << "Advance in buffer" << std::endl;

			if (gameStatesBufferRCurrentIndex + 1 > gameStatesBuffer.getSize() - 1)
				gameStatesBufferRCurrentIndex = 0;
			else
				gameStatesBufferRCurrentIndex++;

			std::cout << std::to_string(gameStatesBuffer.getRelativePosition(gameStatesBufferRCurrentIndex)) << "/" << std::to_string(gameStatesBuffer.getSize()) << std::endl;
		}
		if (event.key.code == sf::Keyboard::Space)
		{

			drawGameStateBuffer = !drawGameStateBuffer;
			if (drawGameStateBuffer)
			{
				std::cout << "Enabled Draw preview buffer State" << std::endl;
				std::cout << std::to_string(gameStatesBuffer.getRelativePosition(gameStatesBufferRCurrentIndex)) << "/" << std::to_string(gameStatesBuffer.getSize()) << std::endl;
				game->stop();
			}
			else
			{
				game->resume();
				std::cout << "Disabled Draw preview buffer State" << std::endl;
			}
		}
	}

	void TBSGameStateRender::drawLayers(sf::RenderWindow& window)
	{
		//Draw Board
		overlaySprites.clear();
		
		// Render Map
		auto* selectedGameStateCopy = &gameStateCopy;
		if (fowSettings.renderFogOfWar)
			selectedGameStateCopy = &gameStateCopyFogOfWar;

		tileMap.update(gameStateCopy, *selectedGameStateCopy, fowSettings.renderFogOfWar, fowSettings.renderType);
		window.draw(tileMap);
		
		//Add selected tileactionSettings.waitingForPosition
		sf::Vector2i mouseGridPos = toGrid(sf::Vector2f(currentMousePos.x, currentMousePos.y));
		
		if (selectedGameStateCopy->isInBounds(Vector2i(mouseGridPos.x, mouseGridPos.y)))
		{
			sf::Texture& texture = assetCache.getTexture("selected");
			sf::Vector2f origin(TILE_ORIGIN_X, TILE_ORIGIN_Y);
			sf::Sprite selectedTile(texture);
		
			selectedTile.setPosition(toISO(mouseGridPos.x, mouseGridPos.y));
			selectedTile.setOrigin(origin);
			overlaySprites.emplace_back(selectedTile);
		}
		
		for (const auto& sprite : overlaySprites)
		{
			window.draw(sprite);
		}
		
		//Draw entities
		entityRenderer.update(*selectedGameStateCopy);
		window.draw(entityRenderer);

		//Draw possible actions
		actionsShapes.clear();
		if(actionsSettings.waitingForPosition)
		{
			for (auto& possibleAction : actionsSettings.actionsHumanPlayer)
			{
				//Check if action is compatible with the selected type and targets
				if (possibleAction.actionTypeID == -1||possibleAction.actionTypeFlags==ActionFlag::ContinuousAction
					||possibleAction.actionTypeFlags == ActionFlag::AbortContinuousAction
					||possibleAction.actionTypeID != actionsSettings.actionTypeSelected)
					continue;

				//Get source
				const auto& actionType = gameStateCopy.getActionTypeConst(possibleAction.actionTypeID);
				
				//Check the source and the selected entity is the same
				if (actionType.sourceType == ActionSourceType::Entity)
				{
					auto& entity = possibleAction.targets[0].getEntity(gameStateCopy);
					if (entity.id != *actionsSettings.selectedEntities.begin())
						continue;

				}
				
				for (auto& actionTarget : possibleAction.targets)
				{
					if (actionTarget.getType() == ActionTarget::Position)
					{
						auto position = actionTarget.getPosition(gameStateCopy);
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
					|| possibleAction.actionTypeFlags == ActionFlag::AbortContinuousAction||
					possibleAction.actionTypeID!=actionsSettings.actionTypeSelected)
					continue;
				
				//Get source
				const auto& actionType = gameStateCopy.getActionTypeConst(possibleAction.actionTypeID);

				//Check the source and the selected entity is the same
				if(actionType.sourceType==ActionSourceType::Entity)
				{
					auto& entity = possibleAction.targets[0].getEntity(gameStateCopy);
					if (entity.id != *actionsSettings.selectedEntities.begin())
						continue;

				}
				
				//Avoid source entity
				for (int i = 1; i < possibleAction.targets.size(); ++i)
				{
					if (possibleAction.targets[i].getType() == ActionTarget::EntityReference)
					{
						auto position = possibleAction.targets[i].getPosition(gameStateCopy);
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

	void TBSGameStateRender::createHUD(sf::RenderWindow& window)
	{
		//ImGui::ShowDemoWindow();
		createWindowInfo();
		createWindowUnits();
		createWindowActions();
		createWindowPlayerParameters();
		
		createEntityInformation(window);
		createWindowFogOfWar();
		createActionBar(window);
	}

	void TBSGameStateRender::createWindowInfo() const
	{
		ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
		ImGui::Begin("Info window");

		std::string gameTurns = "Tick:" + std::to_string(gameStateCopy.currentTick) + " Player Turn: " + std::to_string(gameStateCopy.currentPlayer) + " FPS: " + std::to_string(m_fps);
		ImGui::Text(gameTurns.c_str());

		std::string fpsInfo = "FPS: " + std::to_string(0);
		ImGui::Text(fpsInfo.c_str());

		std::string mousePosInfo = "IMGUI: " + std::to_string((int)ImGui::GetMousePos().x) + "," + std::to_string((int)ImGui::GetMousePos().y) +
			" SFML: " + std::to_string((int)currentMousePos.x) + "," + std::to_string((int)currentMousePos.y);
		ImGui::Text(mousePosInfo.c_str());

		if (ImGui::Button("Close Game"))
		{
			game->end();
		}

		ImGui::End();
	}

	void TBSGameStateRender::createWindowFogOfWar()
	{
		ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
		ImGui::Begin("Fog of War window");

		if(Widgets::fowController(gameStateCopy, fowSettings))
		{
			// Selected player changed -> Re-Apply FogOfWar
			gameStateCopyFogOfWar = game->getStateCopy();
			gameStateCopyFogOfWar.applyFogOfWar(fowSettings.selectedPlayerID);
		}

		ImGui::End();
	}

	void TBSGameStateRender::createTopBar() const
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Resources"))
			{
				if (ImGui::MenuItem("Test1", "text"))
				{
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Resources2"))
			{
				if (ImGui::MenuItem("Test1", "text"))
				{
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}


	void TBSGameStateRender::createEntityInformation(sf::RenderWindow& window)
	{
		if(!actionsSettings.selectedEntities.empty())
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
			ImGui::SetNextWindowPos(ImVec2((0), window.getSize().y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
			ImGui::Begin("Entity Information", NULL, window_flags);

			auto* selectedEntity = gameStateCopy.getEntity(*actionsSettings.selectedEntities.begin());
			auto entityType = gameStateCopy.getEntityType(selectedEntity->typeID);
			
			ImGui::Text(entityType.name.c_str());
			ImGui::Columns(2, "mixed");
			ImGui::SetColumnWidth(0, 100.0f);
			
			ImGui::Separator();
		
			//Add units
			sf::Texture& texture = assetCache.getTexture(entityType.name);

			ImGui::Image(texture, ImVec2(100, 100), sf::Color::White, sf::Color::Transparent);
			
			
			ImGui::NextColumn();
			ImGui::Text("Parameters: ");
			
			int parameterIndex=0;
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
			ImGui::BeginChild("help", ImVec2(0, 80), true,child_flags);


			for (auto &entity : gameStateCopy.getPlayerEntities(fowSettings.selectedPlayerID))
			{
				//Check if entity have sprite
				auto entityType = gameStateCopy.getEntityType(entity->typeID);
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
	
	void TBSGameStateRender::createActionBar(sf::RenderWindow& window)
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
		ImGui::SetNextWindowPos(ImVec2((window.getSize().x/2), window.getSize().y/1.1),ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(0, 0));

		ImGui::Begin("Bottom Bar", NULL, window_flags);

		ImGui::Separator();

		ImGui::Text("Actions");
		
		//Ask widget to get		
		auto actionsToExecute = getWidgetResult(gameStateCopy, actionsSettings, getPlayerID());

		if (!actionsToExecute.empty())
			playAction(actionsToExecute.front());
		
		ImGui::Separator();
		ImGui::End();
	}

	void TBSGameStateRender::createWindowUnits()
	{
		ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 150), ImGuiCond_FirstUseEver);
		ImGui::Begin("Entities");

		ImGui::BeginChild("Scrolling");

		auto& units = gameStateCopy.entities;

		for (auto& unit : units)
		{
			auto& type = gameStateCopy.getEntityType(unit.typeID);
			std::string unitInfo;
			unitInfo = type.name + " " + std::to_string(unit.id) + " PID: " + std::to_string(unit.ownerID);
			ImGui::Text(unitInfo.c_str());
		}

		ImGui::EndChild();
		ImGui::End();
	}

	void TBSGameStateRender::createWindowActions()
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
					if(action.targets[0].getType()==ActionTarget::EntityReference)
					{
						//We need to find the continues action name that will abort
						auto& sourceEntity = gameStateCopy.getEntityConst(action.targets[0].getEntityID());
						for (auto& continueAction : sourceEntity.continuousAction)
						{
							if (continueAction.continuousActionID == action.continuousActionID)
							{
								const ActionType& actionType = gameStateCopy.getActionType(continueAction.actionTypeID);
								actionInfo += " Abort " + actionType.name;
							}
						}
					}
					else
					{
						//We need to find the continues action name that will abort
						auto& sourcePlayer = action.targets[0].getPlayer(gameStateCopy);
						for (auto& continueAction : sourcePlayer.continuousAction)
						{
							if (continueAction.continuousActionID == action.continuousActionID)
							{
								const ActionType& actionType = gameStateCopy.getActionType(continueAction.actionTypeID);
								actionInfo += " Abort " + actionType.name;
							}
						}
					}
					

					
				}					
				else
					actionInfo +=" SpecialAction";				
			}
			else
			{
				const ActionType& actionType = gameStateCopy.getActionType(action.actionTypeID);

				actionInfo += " " + actionType.name;

				//TODO Clean this :D IS TEMPORAL

				for (auto& targetType : action.targets)
				{
					switch (targetType.getType())
					{
					case ActionTarget::Position:
						actionInfo += " x:" + std::to_string((int)targetType.getPosition(gameStateCopy).x) + ",y:" + std::to_string((int)targetType.getPosition(gameStateCopy).y);
						break;
					case ActionTarget::EntityReference:
						actionInfo += gameStateCopy.getEntityType(gameStateCopy.getEntity(targetType.getEntityID())->typeID).name;
						break;
					case ActionTarget::PlayerReference:
						actionInfo += " Player: " + std::to_string(getPlayerID());
						break;
					case ActionTarget::TechnologyReference:
						actionInfo += " Technology: " + gameStateCopy.technologyTreeCollection->getTechnology(targetType.getTechnologyID()).name;
						break;
					case ActionTarget::EntityTypeReference:
						actionInfo += " Entity: " + targetType.getEntityType(gameStateCopy).name;
					case ActionTarget::ContinuousActionReference:
						break;
					}					
				}

			}
			
			index++;			

			if (ImGui::Button(actionInfo.c_str()))
			{
				playAction(action);
				break;
			}
		}

		ImGui::EndGroup();

		ImGui::EndChild();
		ImGui::End();
	}

	void TBSGameStateRender::createWindowPlayerParameters() const
	{
		ImGui::SetNextWindowSize(ImVec2(100, 150), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_FirstUseEver);
		ImGui::Begin("PlayerParameters");
		ImGui::BeginChild("Scrolling");
		ImGui::BeginGroup();

		const auto* player = gameStateCopy.getPlayer(fowSettings.selectedPlayerID);
		for (const auto& parameter : *gameStateCopy.playerParameterTypes)
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

}
