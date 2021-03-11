#include <RTSGameStateRender.h>
#include <Stratega/ForwardModel/Action.h>
#include <CircularBuffer.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iomanip>
#include <sstream>
#include <GridUtils.h>

#include <Configuration/RenderConfig.h>
#include <Widgets/FogOfWarController.h>

namespace SGA
{
	RTSGameStateRender::RTSGameStateRender(SGA::RTSGame& game, const GameConfig& gameConfig, const RenderConfig& renderConfig, int playerID) :
		GameStateRenderer{ playerID },
		game(&game),
		gameStateCopy(game.getStateCopy()),
		gameStatesBuffer(50)
	{
		init(gameConfig, renderConfig);
		
		//Initialize gameStateFog and apply fog to it		
		gameStateCopyFogOfWar = gameStateCopy;
		gameStateCopyFogOfWar.applyFogOfWar(fowSettings.selectedPlayerID);
	}

	void RTSGameStateRender::init()
	{
		GameStateRenderer::init();
	}

	void RTSGameStateRender::onGameStateAdvanced()
	{
		std::lock_guard<std::mutex> lockGuard(mutexRender);
		gameStateCopy = game->getStateCopy();
		gameStateCopyFogOfWar = game->getStateCopy();
		gameStateCopyFogOfWar.applyFogOfWar(fowSettings.selectedPlayerID);

		//Update actions
		actionsSettings.actionsHumanPlayer = game->getForwardModel().generateActions(gameStateCopyFogOfWar, getPlayerID());

		//Add state to buffer
		gameStatesBuffer.add(gameStateCopy);
		gameStatesBufferRCurrentIndex = gameStatesBuffer.getFront();


		//Execute actions
		for (auto it = actionsToPlay.begin(); it != actionsToPlay.end();) {
			if (!it->second.validate(gameStateCopyFogOfWar))
			{
				it = actionsToPlay.erase(it);
			}
			else
			{
				game->executeAction(it->second);
				it++;
			}
		}
	}

	void RTSGameStateRender::init(const GameConfig& gameConfig, const RenderConfig& renderConfig)
	{
		//Need to activate the context before adding new textures
		ctx.setActive(true);

		tileMap.init(gameStateCopy, gameConfig, renderConfig);
		entityRenderer.init(gameStateCopy, gameConfig, renderConfig);

		// Load textures
		for (const auto& namePathPair : renderConfig.entitySpritePaths)
		{
			assetCache.loadTexture(namePathPair.first, namePathPair.second);
		}
		
		// TODO Depends on location of configuration file, how to prevent that?
		assetCache.loadTexture("circleCollider", "../../GUI/Assets/Tiles/circleCollider.png");
		assetCache.loadTexture("boxCollider", "../../GUI/Assets/Tiles/boxCollider.png");
		assetCache.loadFont("font", "../../GUI/Assets/arial.ttf");

		//Create new renderTexture
		if (!renderMinimapTexture.create(2000, 2000))
		{
			// error...
		}
	}

	void RTSGameStateRender::initializeView(sf::RenderWindow& window) const
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

	void RTSGameStateRender::render()
	{
		sf::RenderWindow window(sf::VideoMode(1200, 800), "Strategy Game Engine", sf::Style::Default | sf::Style::Titlebar);
		window.setActive(true);

		ImGui::SFML::Init(window);

		initializeView(window);

		//FPS stuff
		m_fps = 0;
		int m_fpscount = 0;
		sf::Clock clock;

		//Set Minimap View
		//view2.setViewport(sf::FloatRect(0.75f, 0.75f, 0.25f, 0.25f));
		//view2.setCenter((float)(window.getSize().x / 2), (float)(window.getSize().y / 2));
		//view2.setSize(window.getDefaultView().getSize()); // Reset the size	
		//view2.zoom(8);
		//

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

				//Render Game view
				drawLayers(window);


				if (dragging)
				{
					auto currentPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
					auto diff = oldPos - currentPos;
					auto yLeft = std::min(currentPos.y, oldPos.y);
					auto xLeft = std::min(currentPos.x, oldPos.x);
					sf::RectangleShape selectedArea({ std::abs(diff.x), std::abs(diff.y) });
					selectedArea.setFillColor(sf::Color::Transparent);
					selectedArea.setOutlineColor(sf::Color::White);
					selectedArea.setOutlineThickness(10);
					selectedArea.setPosition(xLeft, yLeft);
					window.draw(selectedArea);
				}

				ImGui::SFML::Update(window, deltaClock.restart());

				createHUD(window);
			}

			ImGui::SFML::Render(window);
			window.display();
		}
		game->end();
		window.close();
	}

	void RTSGameStateRender::handleInput(sf::RenderWindow& window)
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

	void RTSGameStateRender::windowClosed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)const
	{
		window.close();
		game->end();
	}
	void RTSGameStateRender::mouseScrolled(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
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
	void RTSGameStateRender::mouseButtonReleased(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
	{
		// Mouse button is released, no longer move
		if (event.mouseButton.button == sf::Mouse::Middle) {
			moving = false;
		}

		if (event.mouseButton.button == sf::Mouse::Left && ((fowSettings.renderFogOfWar && (getPlayerID() == fowSettings.selectedPlayerID)) || !fowSettings.renderFogOfWar))
		{
			dragging = false;

			sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			auto worldPos = toGrid(pos);
			auto movementDir = pos - oldPos;
			auto movementDistance = std::sqrt(movementDir.x * movementDir.x + movementDir.y * movementDir.y);

			if (movementDistance <= 10.0)
			{
				// The user clicked somewhere
				if (!actionsSettings.selectedEntities.empty())
				{
					auto* unit = gameStateCopy.getEntity(SGA::Vector2f(worldPos.x, worldPos.y), 0.5);

					if (unit)
					{
						//we click on someone
						if (actionsSettings.waitingForEntity)
						{
							auto gridPos = toGridFloat(pos);
							assignEntity(gameStateCopy, actionsSettings, unit->id);
						}
						else
						{
							actionsSettings.selectedEntities.clear();
						}
					
					}
					else
					{
						if(actionsSettings.waitingForPosition)
						{
							auto gridPos = toGridFloat(pos);
							assignPosition(gameStateCopy, actionsSettings,{gridPos.x,gridPos.y});
						}
						else
						{
							actionsSettings.selectedEntities.clear();
						}						
					}
				}
				else
				{
					if (actionsSettings.waitingForPosition)
					{
						auto gridPos = toGrid(pos);
						assignPosition(gameStateCopy, actionsSettings, { (float)gridPos.x,(float)gridPos.y });
					}
					else
					{
						actionsSettings.selectedEntities.clear();
						actionsSettings.reset();
					}
				}
			}
			else
			{
				// The user selected an area
				auto prevMouseWorldPos = oldPos;
				auto xLeft = std::min(prevMouseWorldPos.x, pos.x);
				auto xRight = std::max(prevMouseWorldPos.x, pos.x);
				auto yLeft = std::min(prevMouseWorldPos.y, pos.y);
				auto yRight = std::max(prevMouseWorldPos.y, pos.y);

				for (auto& unit : gameStateCopy.entities)
				{
					sf::Vector2f screenPos = toISO(unit.position.x, unit.position.y);
					if (screenPos.x > xLeft && screenPos.x < xRight && screenPos.y > yLeft && screenPos.y < yRight)
					{
						if (unit.ownerID == getPlayerID())
							actionsSettings.selectedEntities.emplace(unit.id);
					}
				}
			}
		}
	}
	void RTSGameStateRender::mouseButtonPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
	{
		// Mouse button is pressed, get the position and set moving as active
		if (event.mouseButton.button == sf::Mouse::Left && ((fowSettings.renderFogOfWar && (getPlayerID() == fowSettings.selectedPlayerID)) || !fowSettings.renderFogOfWar))
		{
			oldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			dragging = true;
		}
		// Mouse button is pressed, get the position and set moving as active
		if (event.mouseButton.button == sf::Mouse::Middle)
		{
			moving = true;
			oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
		}
	}
	void RTSGameStateRender::mouseMoved(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
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
	void RTSGameStateRender::keyPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
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
		view.setCenter(view.getCenter() + movementDir);
		window.setView(view);

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

	void RTSGameStateRender::drawLayers(sf::RenderWindow& window)
	{
		// Remove entities that do not exist anymore
		for(auto i = actionsSettings.selectedEntities.begin(); i != actionsSettings.selectedEntities.end();)
		{
			if(gameStateCopy.getEntity(*i) == nullptr)
			{
				actionsSettings.selectedEntities.erase(i);
			}
			else
			{
				++i;
			}
		}
		renderMinimapTexture.clear();

		// Render Map
		auto* selectedGameStateCopy = &gameStateCopy;
		if (fowSettings.renderFogOfWar)
			selectedGameStateCopy = &gameStateCopyFogOfWar;

		tileMap.update(gameStateCopy, gameStateCopyFogOfWar, fowSettings.renderFogOfWar, fowSettings.renderType);
		window.draw(tileMap);
		renderMinimapTexture.draw(tileMap);
		
		//Draw entities
		entityRenderer.update(*selectedGameStateCopy);
		window.draw(entityRenderer);


		//Check if units are selected
		for (const auto& unit : selectedGameStateCopy->entities)
		{
			if (isSelected(unit.id))
			{
				auto& selectionTexture = assetCache.getTexture("circleCollider");
				sf::Sprite selectionSprite(selectionTexture);
				selectionSprite.setOrigin(selectionTexture.getSize().x / 2, selectionTexture.getSize().y / 2);
				selectionSprite.setPosition(toISO(unit.position.x, unit.position.y) + sf::Vector2f{ TILE_WIDTH_HALF, TILE_HEIGHT_HALF });
				selectionSprite.setColor(sf::Color::Blue);
				window.draw(selectionSprite);
			}
		}

		if (drawDebug) {
			const dtNavMesh* mesh = selectedGameStateCopy->navigation->m_navMesh;

			if (mesh)
			{
				//Draw navmesh polygons
				for (int i = 0; i < selectedGameStateCopy->navigation->m_navMesh->getMaxTiles(); ++i)
				{
					const dtMeshTile* tile = mesh->getTile(i);

					if (!tile->header)
						continue;

					for (int i = 0; i < tile->header->polyCount; ++i)
					{
						const dtPoly* p = &tile->polys[i];
						if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
							continue;

						const dtPolyDetail* pd = &tile->detailMeshes[i];

						//Draw polygon
						for (int j = 0; j < pd->triCount; ++j)
						{
							const unsigned char* t = &tile->detailTris[(pd->triBase + j) * 4];

							sf::ConvexShape polygon;

							//Adjust to offset from isometric
							polygon.move(TILE_WIDTH_HALF, 0);

							polygon.setOutlineColor(sf::Color::Black);
							polygon.setFillColor(sf::Color(20, 140, 240, 64));
							polygon.setOutlineThickness(1);
							polygon.setPointCount(3);
							for (int k = 0; k < 3; ++k)
							{
								if (t[k] < p->vertCount)
								{
									float* pos = &tile->verts[p->verts[t[k]] * 3];
									polygon.setPoint(k, toISO(pos[0], pos[2]));
								}
							}
							window.draw(polygon);
						}
					}

				}
			}
		}

		//Draw paths of units
		for (auto& unit : selectedGameStateCopy->entities)
		{
			//Check if has a valid path
			if (unit.path.m_nstraightPath > 0)
			{
				//Draw path lines
				for (int i = 0; i < unit.path.m_nstraightPath - 1; ++i)
				{
					auto v1 = toISO(unit.path.m_straightPath[i * 3], unit.path.m_straightPath[i * 3 + 2]);
					auto v2 = toISO(unit.path.m_straightPath[(i + 1) * 3], unit.path.m_straightPath[(i + 1) * 3 + 2]);

					//Adjust to sprites offset
					v1.x += TILE_WIDTH_HALF;
					v2.x += TILE_WIDTH_HALF;

					sf::Vertex line[] =
					{
						sf::Vertex(v1),
						sf::Vertex(v2)
					};

					window.draw(line, 2, sf::Lines);
				}

				//Draw path points
				for (int i = 0; i < unit.path.m_nstraightPath; ++i)
				{
					sf::CircleShape pathCircle;
					float v1 = unit.path.m_straightPath[i * 3];
					float v2 = unit.path.m_straightPath[i * 3 + 2];

					auto pos = toISO(v1, v2);
					pathCircle.setPosition(pos);
					pathCircle.setOrigin(10, 10);
					pathCircle.setFillColor(sf::Color::Black);
					pathCircle.setRadius(20);
					pathCircle.move(TILE_WIDTH_HALF, 0);
					window.draw(pathCircle);
				}

				////Paint Start and End circles
				//sf::CircleShape temp;
				//temp.setOrigin(10, 10);
				//temp.setPosition(toISO(unit.position.x, unit.position.y));
				//temp.move(TILE_WIDTH_HALF, 0);
				//temp.setFillColor(sf::Color::Green);
				//temp.setRadius(20);
				//window.draw(temp);

				//temp.setPosition(toISO(unit.executingAction.targetPosition.x, unit.executingAction.targetPosition.y));
				//temp.move(TILE_WIDTH_HALF, 0);
				//temp.setFillColor(sf::Color::Red);
				//temp.setRadius(20);
				//window.draw(temp);
			}
		}

		renderMinimapTexture.display();
	}

	void RTSGameStateRender::createHUD(sf::RenderWindow& window)
	{
		/*ImGui::ShowDemoWindow();*/		
		createTopBar();
		createWindowInfo(window);
		createWindowUnits();
		createWindowNavMesh();
		createBottomBar(window);
		createWindowFogOfWar();
		createWindowPlayerParameters();
		createWindowActionList();
	}

	
	void RTSGameStateRender::createBottomBar(sf::RenderWindow& window)
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags += ImGuiWindowFlags_NoTitleBar;
		window_flags += ImGuiWindowFlags_NoScrollbar;
		window_flags += ImGuiWindowFlags_MenuBar;
		//window_flags+= ImGuiWindowFlags_NoMove;
		window_flags += ImGuiWindowFlags_NoResize;
		window_flags += ImGuiWindowFlags_NoCollapse;
		window_flags += ImGuiWindowFlags_NoNav;
		//window_flags+= ImGuiWindowFlags_NoBackground;
		window_flags += ImGuiWindowFlags_NoBringToFrontOnFocus;

		// We specify a default position/size in case there's no data in the .ini file.
		// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
		ImGui::SetNextWindowPos(ImVec2(0, 590));
		ImGui::SetNextWindowSize(ImVec2(window.getSize().x, 300));

		//ImGui::SetNextWindowContentSize(ImVec2(600, 700));
		ImGui::Begin("Bottom Bar", NULL, window_flags);

		ImGui::Columns(3, "mixed");
		ImGui::SetColumnWidth(0, 250.0f);
		ImGui::SetColumnWidth(1, 600.0f);
		ImGui::SetColumnWidth(2, 250);
		ImGui::Separator();
		
		//Ask widget to get		
		auto actionsToExecute = getWidgetResult(gameStateCopy, actionsSettings, getPlayerID());

		//if(!actionsToExecute.empty())
			playActions(actionsToExecute);

		ImGui::NextColumn();
		ImGui::Text("Entities");
		int elementNumber = 0;

		for (auto &entity : actionsSettings.selectedEntities)
		{
			ImGui::PushID(elementNumber);
			if ((elementNumber++ % 8) != 0) ImGui::SameLine();

			//Check if entity have sprite
			auto entityType = gameStateCopy.getEntityType(gameStateCopy.getEntity(entity)->typeID);
			//Add units
			sf::Texture& texture = assetCache.getTexture(entityType.name);

			if (ImGui::ImageButton(texture, ImVec2(50, 50), -10))
			{
			}

			ImGui::PopID();
		}


		ImGui::NextColumn();

		ImGui::Text("Minimap");

		/*renderMinimapTexture.get.create(window.getSize().x, window.getSize().y);
		texture.update(window);*/

	/*	sf::Sprite sprite;

		sprite.setTexture(renderMinimapTexture.getTexture());

		sprite.scale(0.1, -0.1);
		sprite.rotate(20);*/
		//ImGui::Image(renderMinimapTexture.getTexture(), sf::Vector2f(250, 250));

		ImGui::NextColumn();

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::End();
	}

	void RTSGameStateRender::createTopBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Resources"))
			{
				if (ImGui::MenuItem("Test1", "text")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Resources2"))
			{
				if (ImGui::MenuItem("Test1", "text")) {}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
	
	void RTSGameStateRender::createWindowInfo(sf::RenderWindow& window)
	{
		ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
		ImGui::Begin("Info window");


		std::string fpsInfo = "FPS: " + std::to_string(0);
		ImGui::Text(fpsInfo.c_str());
		ImGui::Separator();
		std::string mousePosInfo = "IMGUI: " + std::to_string((int)ImGui::GetMousePos().x) + "," + std::to_string((int)ImGui::GetMousePos().y) +
			" SFML: " + std::to_string((int)currentMousePos.x) + "," + std::to_string((int)currentMousePos.y);
		ImGui::Text(mousePosInfo.c_str());

		sf::Vector2f mousePos(currentMousePos.x, currentMousePos.y);
		mousePosInfo = "GRID: " + std::to_string(toGridFloat(mousePos).x) + "," + std::to_string(toGridFloat(mousePos).y);

		ImGui::Text(mousePosInfo.c_str());

		ImGui::Separator();
		if (ImGui::Button("Close Game"))
		{
			game->end();
		}
		ImGui::Separator();
		if (ImGui::Button("Draw Debug"))
		{
			drawDebug = !drawDebug;
		}

		ImGui::Separator();
		if (ImGui::Button("Screenshot"))
		{
			sf::Texture texture;
			texture.create(window.getSize().x, window.getSize().y);
			texture.update(window);
			std::time_t rawtime;
			std::time(&rawtime);
			const std::tm* timeinfo = std::localtime(&rawtime);

			char yyyymmdd[16];
			std::strftime(yyyymmdd, sizeof(yyyymmdd), "%Y%m%d", timeinfo);
			std::string fileName = "screenshot" + std::string(yyyymmdd) + ".png";
			if (texture.copyToImage().saveToFile(fileName))
			{
				std::cout << "screenshot saved to " << fileName << std::endl;
			}
		}

		ImGui::End();
	}

	void RTSGameStateRender::createWindowUnits()
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

	void RTSGameStateRender::createWindowActionList()
	{
		ImGui::SetNextWindowSize(ImVec2(200, 150), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin("Actions");
		ImGui::BeginChild("Scrolling");
		ImGui::BeginGroup();

		int index = 0;
		auto actionsHumanCanPlay = game->getForwardModel().generateActions(gameStateCopyFogOfWar, getPlayerID());

		for (auto action : actionsHumanCanPlay)
		{

			std::string actionInfo = std::to_string(index);
			if (action.actionTypeID == -1)
			{
				if (action.actionTypeFlags == ActionFlag::AbortContinuousAction)
				{
					if (action.targets[0].getType() == ActionTarget::EntityReference)
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
					actionInfo += " SpecialAction";
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
				game->executeAction(action);
				//playAction(action);
				break;
			}
		}

		ImGui::EndGroup();

		ImGui::EndChild();
		ImGui::End();
	}

	void RTSGameStateRender::createWindowNavMesh()
	{
		ImGui::Begin("Debug Navigation");
		if (ImGui::Button("Generate NavMesh"))
		{
			game->navigationConfig = config;
			game->shouldUpdateNavmesh = true;
		}

		// TODO add more precision to the navmesh generation
		/*ImGui::Text("Rasterization");
		ImGui::SliderFloat("Cell Size", &config.m_cellSize, 0.1f, 1.0f);
		ImGui::SliderFloat("Cell Height", &config.m_cellHeight, 0.1f, 1.0f);*/

		ImGui::Separator();
		ImGui::Text("Agent");
		//ImGui::SliderFloat("Height", &config.m_agentHeight, 0.1f, 5.0f);
		ImGui::SliderFloat("Radius", &config.m_agentRadius, 0.0f, 5.0f);
		/*ImGui::SliderFloat("Max Climb", &config.m_agentMaxClimb, 0.1f, 5.0f);
		ImGui::SliderFloat("Max Slope", &config.m_agentMaxSlope, 0.0f, 90.0f);*/

		ImGui::Separator();
		ImGui::Text("Region");
		ImGui::SliderFloat("Min Region Size", &config.m_regionMinSize, 0.0f, 150.0f);
		ImGui::SliderFloat("Merged Region Size", &config.m_regionMergeSize, 0.0f, 150.0f);

		ImGui::Separator();
		ImGui::Text("Erode mesh");
		ImGui::Checkbox("Erode Walkable Area", &config.m_erodeWalkableArea);

		ImGui::Separator();
		ImGui::Text("Partitioning");
		ImGui::RadioButton("Watershed", &config.m_partitionType, SAMPLE_PARTITION_WATERSHED);
		ImGui::RadioButton("Monotone", &config.m_partitionType, SAMPLE_PARTITION_MONOTONE);
		ImGui::RadioButton("Layers", &config.m_partitionType, SAMPLE_PARTITION_LAYERS);

		ImGui::Separator();
		ImGui::Text("Filtering");
		ImGui::Checkbox("Low Hanging Obstacles", &config.m_filterLowHangingObstacles);
		ImGui::Checkbox("Ledge Spans", &config.m_filterLedgeSpans);
		ImGui::Checkbox("Walkable Low Height Spans", &config.m_filterWalkableLowHeightSpans);

		ImGui::Separator();
		ImGui::Text("Polygonization");
		ImGui::SliderFloat("Max Edge Length", &config.m_edgeMaxLen, 0.0f, 50.0f);
		ImGui::SliderFloat("Max Edge Error", &config.m_edgeMaxError, 0.1f, 3.0f);
		ImGui::SliderFloat("Verts Per Poly", &config.m_vertsPerPoly, 3.0f, 12.0f);

		ImGui::Separator();
		ImGui::Text("Detail Mesh");
		ImGui::SliderFloat("Sample Distance", &config.m_detailSampleDist, 0.0f, 16.0f);
		ImGui::SliderFloat("Max Sample Error", &config.m_detailSampleMaxError, 0.0f, 16.0f);

		ImGui::End();
	}

	void RTSGameStateRender::createWindowFogOfWar()
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

	void RTSGameStateRender::createWindowPlayerParameters() const
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