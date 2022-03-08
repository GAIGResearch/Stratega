#include <Stratega/GUI/RTSGameRenderer.h>
#include <Stratega/GUI/GridUtils.h>
#include <Stratega/Configuration/RenderConfig.h>
#include <Stratega/Configuration/GameConfig.h>

#include <SFML/Window.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iomanip>
#include <sstream>

#include<Stratega/Utils/warnings.h>

DISABLE_WARNING_PUSH
#if defined(__clang__)    
DISABLE_WARNING_FORMAT
#elif defined(__GNUC__)
DISABLE_WARNING_FORMAT
#endif
namespace SGA
{
	RTSGameRenderer::RTSGameRenderer(SGA::Vector2i& newResolution)
		: state(),
		fowState(),
		config(nullptr),
		assignment(),		
		window(sf::VideoMode(static_cast<unsigned>(newResolution.x), static_cast<unsigned>(newResolution.y)), "Stratega GUI", sf::Style::Default | sf::Style::Titlebar),
		pointOfViewPlayerID(NO_PLAYER_ID),
		fowSettings(),
		zoomValue(5.f),
		dragging(false)
	{
		fowSettings.selectedPlayerID = pointOfViewPlayerID;
		fowSettings.renderFogOfWar = false;
		fowSettings.renderType = FogRenderType::Fog;

		// Initialize View
		sf::View view = window.getView();
		view.setCenter(static_cast<float>(window.getSize().x / 2.), static_cast<float>(window.getSize().y / 2.));
		view.setSize(window.getDefaultView().getSize()); // Reset the size
		view.zoom(zoomValue); // Apply the zoom level (this transforms the view)
		window.setView(view);
	}

	void RTSGameRenderer::init(const GameState& initialState, const GameConfig& gameConfig)
	{
		config = &gameConfig;
		
		fowSettings.renderFogOfWar = config->applyFogOfWar;

		// Load textures
		for (const auto& namePathPair : gameConfig.renderConfig->entitySpritePaths)
		{
			assetCache.loadTexture(namePathPair.first, namePathPair.second);
		}

		assetCache.loadTexture("circleCollider", gameConfig.renderConfig->entityCircleColliderPath);

		assetCache.loadFont("font", gameConfig.renderConfig->fontPath);
		
		
		//std::string circleColliderPath = "../..//GUI/Assets/Tiles/circleCollider.png";

		//
		//filePath = circleColliderPath;
		//tmp = ghc::filesystem::current_path();
		//ghc::filesystem::current_path(ghc::filesystem::canonical(ghc::filesystem::path(config->yamlPath).parent_path()));
		//filePath = canonical(filePath);
		//current_path(tmp);
		//assetCache.loadTexture("circleCollider", filePath.string());


		///*assetCache.loadTexture("circleCollider", "./GUI/Assets/Tiles/circleCollider.png");
		//assetCache.loadTexture("boxCollider", "./GUI/Assets/Tiles/boxCollider.png");*/

		tileMap.init(initialState, gameConfig, *gameConfig.renderConfig);
		entityRenderer.init(initialState, gameConfig, *gameConfig.renderConfig);

		ImGui::SFML::Init(window);

		update(initialState);
	}

	void RTSGameRenderer::update(const GameState& newState)
	{
		this->state = newState;
		this->fowState = newState;
		this->fowState.applyFogOfWar(fowSettings.selectedPlayerID);
		assignment.clear();

		GameState currentState;
		if (config->applyFogOfWar)
			currentState = newState;
		else
			currentState = fowState;

		auto it = futureActionsToPlay.begin();
		//Assign entity actions if they are still valid
		while (it != futureActionsToPlay.end())
		{
			if (!it->validate(currentState)) {

				it = futureActionsToPlay.erase(it);
			}
			else
			{
				assignment.assignActionOrReplace(*it);
				++it;
			}
		}
	}

	void RTSGameRenderer::render()
	{
		handleInput();

		window.clear();
		renderLayers();

		// Handle dragging
		if (dragging)
		{
			auto currentPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			auto diff = oldMousePosition - currentPos;
			auto yLeft = std::min(currentPos.y, oldMousePosition.y);
			auto xLeft = std::min(currentPos.x, oldMousePosition.x);
			sf::RectangleShape selectedArea({ std::abs(diff.x), std::abs(diff.y) });
			selectedArea.setFillColor(sf::Color::Transparent);
			selectedArea.setOutlineColor(sf::Color::White);
			selectedArea.setOutlineThickness(10);
			selectedArea.setPosition(xLeft, yLeft);
			window.draw(selectedArea);
		}
		
		ImGui::SFML::Update(window, deltaClock.restart());
		createHUD();
		ImGui::SFML::Render(window);
		window.display();
	}

	void RTSGameRenderer::handleInput()
	{
		//Get current View to edit it and apply changes
		sf::View view = window.getView();

		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if(ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered())
				return;

			switch (event.type)
			{
			case sf::Event::Closed: {endGameRequested = true; break; }
			case sf::Event::MouseWheelScrolled: { mouseScrolled(event, view, window); break; }
			case sf::Event::MouseButtonReleased: { mouseButtonReleased(event, view, window); break; }
			case sf::Event::MouseButtonPressed: { mouseButtonPressed(event, view, window); break; }
			case sf::Event::MouseMoved: {/* Ignore mouse movement unless a button is pressed*/ mouseMoved(event, view, window); 	break; }
			case sf::Event::KeyPressed: {keyPressed(event, view, window); break;	}
			default:  break;
			}

		}
	}
	
	void RTSGameRenderer::mouseScrolled(const sf::Event& event, sf::View& view, sf::RenderWindow& /*window*/)
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
	void RTSGameRenderer::mouseButtonReleased(const sf::Event& event, sf::View& /*view*/, sf::RenderWindow& /*window*/)
	{
		// Mouse button is released, no longer move
		if (event.mouseButton.button == sf::Mouse::Middle) {
			moving = false;
		}

		if (event.mouseButton.button == sf::Mouse::Left )
		{
			dragging = false;

			sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			auto worldPos = toGrid(pos);
			auto movementDir = pos - oldMousePosition;
			auto movementDistance = std::sqrt(movementDir.x * movementDir.x + movementDir.y * movementDir.y);

			if (movementDistance <= 10.0)
			{
				// The user clicked somewhere
				if (!actionsSettings.selectedEntities.empty())
				{
					auto* unit = state.getEntity(SGA::Vector2f(worldPos.x, worldPos.y), 0.5);

					if (unit)
					{
						//we click on someone
						if (actionsSettings.waitingForEntity)
						{
							assignEntity(state, actionsSettings, unit->getID());
						}
						else
						{
							actionsSettings.selectedEntities.clear();
						}

					}
					else
					{
						if (actionsSettings.waitingForPosition)
						{
							auto gridPos = toGridFloat(pos);
							assignPosition(state, actionsSettings, { gridPos.x,gridPos.y });
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
						assignPosition(state, actionsSettings, { static_cast<float>(gridPos.x),static_cast<float>(gridPos.y )});
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
				auto prevMouseWorldPos = oldMousePosition;
				auto xLeft = std::min(prevMouseWorldPos.x, pos.x);
				auto xRight = std::max(prevMouseWorldPos.x, pos.x);
				auto yLeft = std::min(prevMouseWorldPos.y, pos.y);
				auto yRight = std::max(prevMouseWorldPos.y, pos.y);

				for (auto& unit : state.getEntities())
				{
					sf::Vector2f screenPos = toISO(unit.x(), unit.y());
					if (screenPos.x > xLeft && screenPos.x < xRight && screenPos.y > yLeft && screenPos.y < yRight)
					{
						if (unit.getOwnerID() == pointOfViewPlayerID)
							actionsSettings.selectedEntities.emplace(unit.getID());
					}
				}
			}
		}
	}
	void RTSGameRenderer::mouseButtonPressed(const sf::Event& event, sf::View& /*view*/, sf::RenderWindow& newWindow)
	{
		// Mouse button is pressed, get the position and set moving as active
		if (event.mouseButton.button == sf::Mouse::Left && ((fowSettings.renderFogOfWar && (pointOfViewPlayerID == fowSettings.selectedPlayerID)) || !fowSettings.renderFogOfWar))
		{
			oldMousePosition = newWindow.mapPixelToCoords(sf::Mouse::getPosition(newWindow));
			dragging = true;
		}
		// Mouse button is pressed, get the position and set moving as active
		if (event.mouseButton.button == sf::Mouse::Middle)
		{
			moving = true;
			oldMousePosition = newWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
		}
	}
	void RTSGameRenderer::mouseMoved(const sf::Event& event, sf::View& view, sf::RenderWindow& newWindow)
	{

		// Determine the new position in world coordinates
		const sf::Vector2f newPos = newWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

		if (!moving)
			return;
		// Determine how the cursor has moved
		// Swap these to invert the movement direction
		const sf::Vector2f deltaPos = oldMousePosition - newPos;

		// Move our view accordingly and update the window
		view.setCenter(view.getCenter() + deltaPos);
		newWindow.setView(view);

		// Save the new position as the old one
		// We're recalculating this, since we've changed the view
		oldMousePosition = newWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
	}
	void RTSGameRenderer::keyPressed(const sf::Event& /*event*/, sf::View& view, sf::RenderWindow& newWindow)
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
		newWindow.setView(view);
	}

	void RTSGameRenderer::renderLayers()
	{
		// Remove entities that do not exist anymore
		for (auto i = actionsSettings.selectedEntities.begin(); i != actionsSettings.selectedEntities.end();)
		{
			if (state.getEntity(*i) == nullptr)
			{
				actionsSettings.selectedEntities.erase(i);
			}
			else
			{
				++i;
			}
		}

		// Render Map
		auto* selectedGameStateCopy = &state;
		if (fowSettings.renderFogOfWar)
			selectedGameStateCopy = &fowState;

		tileMap.update(state, fowState, fowSettings.renderFogOfWar, fowSettings.renderType);
		window.draw(tileMap);

		//Draw entities
		entityRenderer.update(*selectedGameStateCopy);
		window.draw(entityRenderer);


		//Check if units are selected
		for (const auto& unit : selectedGameStateCopy->getEntities())
		{
			if (actionsSettings.isSelected(unit.getID()))
			{
				auto& selectionTexture = assetCache.getTexture("circleCollider");
				sf::Sprite selectionSprite(selectionTexture);
				selectionSprite.setOrigin(static_cast<float>(selectionTexture.getSize().x) / 2.f, static_cast<float>(selectionTexture.getSize().y) / 2.f);
				selectionSprite.setPosition(toISO(unit.x(), unit.y()) + sf::Vector2f{ TILE_WIDTH_HALF, TILE_HEIGHT_HALF });
				selectionSprite.setColor(sf::Color::Blue);
				window.draw(selectionSprite);
			}
		}

		if (drawDebug) {
			const dtNavMesh* mesh = selectedGameStateCopy->getRTSNavigation()->m_navMesh;

			if (mesh)
			{
				//Draw navmesh polygons
				for (int i = 0; i < selectedGameStateCopy->getRTSNavigation()->m_navMesh->getMaxTiles(); ++i)
				{
					const dtMeshTile* tile = mesh->getTile(i);

					if (!tile->header)
						continue;

					for (size_t b = 0; b < static_cast<size_t>(tile->header->polyCount); ++b)
					{
						const dtPoly* p = &tile->polys[b];
						if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
							continue;

						const dtPolyDetail* pd = &tile->detailMeshes[b];

						//Draw polygon
						for (size_t j = 0; j < pd->triCount; ++j)
						{
							const unsigned char* t = &tile->detailTris[static_cast<size_t>((pd->triBase + static_cast<unsigned int>(j)) * 4)];

							sf::ConvexShape polygon;

							//Adjust to offset from isometric
							polygon.move(TILE_WIDTH_HALF, 0);

							polygon.setOutlineColor(sf::Color::Black);
							polygon.setFillColor(sf::Color(20, 140, 240, 64));
							polygon.setOutlineThickness(1);
							polygon.setPointCount(3);
							for (size_t k = 0; k < 3; ++k)
							{
								if (t[k] < p->vertCount)
								{
									float* pos = &tile->verts[static_cast<size_t>(p->verts[static_cast<size_t>(t[k])] * 3)];
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
		for (auto& unit : selectedGameStateCopy->getEntities())
		{
			//Check if has a valid path
			if (unit.getPath().m_nstraightPath > 0)
			{
				//Draw path lines
				for (int i = 0; i < unit.getPath().m_nstraightPath - 1; ++i)
				{
					auto v1 = toISO(unit.getPath().m_straightPath[i * 3], unit.getPath().m_straightPath[i * 3 + 2]);
					auto v2 = toISO(unit.getPath().m_straightPath[(i + 1) * 3], unit.getPath().m_straightPath[(i + 1) * 3 + 2]);

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
				for (int i = 0; i < unit.getPath().m_nstraightPath; ++i)
				{
					sf::CircleShape pathCircle;
					float v1 = unit.getPath().m_straightPath[i * 3];
					float v2 = unit.getPath().m_straightPath[i * 3 + 2];

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
	}

	void RTSGameRenderer::createHUD()
	{
		/*ImGui::ShowDemoWindow();*/
		createTopBar();
		createWindowInfo();
		createWindowUnits();
		createWindowNavMesh();
		createBottomBar();
		createWindowFogOfWar();
		createWindowPlayerParameters();
		createWindowActionList();
	}


	void RTSGameRenderer::createBottomBar()
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
		ImGui::SetNextWindowPos(ImVec2(0, static_cast<float>(window.getSize().y)- static_cast<float>(window.getSize().y)/4));
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>((window.getSize().x)), static_cast<float>(window.getSize().y)/4));

		//ImGui::SetNextWindowContentSize(ImVec2(600, 700));
		ImGui::Begin("Bottom Bar", NULL, window_flags);

		ImGui::Columns(3, "mixed");
		ImGui::SetColumnWidth(0, static_cast<float>(window.getSize().x/4));
		ImGui::SetColumnWidth(1, static_cast<float>((window.getSize().x/4)*2));
		ImGui::SetColumnWidth(2, static_cast<float>(window.getSize().x/4));
		ImGui::Separator();

		//Ask widget to get	
		if (pointOfViewPlayerID != NO_PLAYER_ID)
		{
			auto actionsToExecute = getWidgetResult(fowState, actionsSettings, pointOfViewPlayerID);
			for (auto& action : actionsToExecute)
			{
				if (action.getActionTypeID() == -1 || action.isPlayerAction())
				{
					//If continuous or Player action we assign it directly
					assignment.assignActionOrReplace(action);
				}
				else
				{
					//If is Entity action we save it for future calls
					futureActionsToPlay.emplace_back(action);
				}
			}
		}

		ImGui::NextColumn();
		ImGui::Text("Entities");
		int elementNumber = 0;

		if(checkSelectedEntitiesAvailable(fowState, actionsSettings))
			for (auto& entity : actionsSettings.selectedEntities)
			{
				ImGui::PushID(elementNumber);
				if ((elementNumber++ % 8) != 0) ImGui::SameLine();

				//Check if entity have sprite
				auto& entityType = state.getEntity(entity)->getEntityType();
				//Add units
				sf::Texture& texture = assetCache.getTexture(entityType.getName());

				if(ImGui::ImageButton(texture, sf::Vector2f(50, 50), -10))
				{
				}

				ImGui::PopID();
			}


		ImGui::NextColumn();

		ImGui::Text("Minimap");

		ImGui::NextColumn();

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::End();
	}

	void RTSGameRenderer::createTopBar()
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

	void RTSGameRenderer::createWindowInfo()
	{
		ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
		ImGui::Begin("Info window");
		ImGui::Separator();
		if (ImGui::Button("Draw Debug"))
		{
			drawDebug = !drawDebug;
		}

		ImGui::End();
	}

	void RTSGameRenderer::createWindowUnits()
	{
		ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 150), ImGuiCond_FirstUseEver);
		ImGui::Begin("Entities");

		ImGui::BeginChild("Scrolling");

		for (auto& unit : state.getEntities())
		{
			auto& type = unit.getEntityType();
			std::string unitInfo;
			unitInfo = type.getName() + " " + std::to_string(unit.getID()) + " PID: " + std::to_string(unit.getOwnerID());
			ImGui::Text(unitInfo.c_str());
		}

		ImGui::EndChild();
		ImGui::End();
	}

	void RTSGameRenderer::createWindowActionList()
	{
		ImGui::SetNextWindowSize(ImVec2(200, 150), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin("Actions");
		ImGui::BeginChild("Scrolling");
		ImGui::BeginGroup();

		int index = 0;
		if (pointOfViewPlayerID != NO_PLAYER_ID)
		{
			auto actionsHumanCanPlay = config->forwardModel->generateActions(fowState, pointOfViewPlayerID);

			for (auto action : actionsHumanCanPlay)
			{
				std::string actionInfo = std::to_string(index);
				if (action.getActionTypeID() == -1)
				{
					if (action.getActionFlag() == ActionFlag::AbortContinuousAction)
					{
						if (action.getTargets()[0].getType() == ActionTarget::EntityReference)
						{
							//We need to find the continues action name that will abort
							auto& sourceEntity = *state.getEntityConst(action.getTargets()[0].getEntityID());
							for (const auto& continueAction : sourceEntity.getContinuousActions())
							{
								if (continueAction.getContinuousActionID() == action.getContinuousActionID())
								{
									const ActionType& actionType = continueAction.getActionType();
									actionInfo += " Abort " + actionType.getName();
								}
							}
						}
						else
						{
							//We need to find the continues action name that will abort
							auto& sourcePlayer = action.getTargets()[0].getPlayer(state);
							for (auto& continueAction : sourcePlayer.getContinuousActions())
							{
								if (continueAction.getContinuousActionID() == action.getContinuousActionID())
								{
									const ActionType& actionType = continueAction.getActionType();
									actionInfo += " Abort " + actionType.getName();
								}
							}
						}
					}
					else
						actionInfo += " SpecialAction";
				}
				else
				{
					const ActionType& actionType = action.getActionType();

					actionInfo += " " + actionType.getName();

					//TODO Clean this :D IS TEMPORAL
					for (auto& targetType : action.getTargets())
					{
						switch (targetType.getType())
						{
						case ActionTarget::Position:
							actionInfo += " x:" + std::to_string(static_cast<int>(targetType.getPosition(state).x)) + ",y:" + std::to_string(static_cast<int>(targetType.getPosition(state).y));
							break;
						case ActionTarget::EntityReference:
							actionInfo += state.getEntity(targetType.getEntityID())->getEntityType().getName();
							break;
						case ActionTarget::PlayerReference:
							actionInfo += " Player: " + std::to_string(pointOfViewPlayerID);
							break;
						case ActionTarget::TechnologyReference:
							actionInfo += " Technology: " + state.getGameInfo()->getTechnologyTreeCollection().getTechnology(targetType.getTechnologyID()).name;
							break;
						case ActionTarget::EntityTypeReference:
							actionInfo += " Entity: " + targetType.getEntityType(state).getName();
							break;
						case ActionTarget::ContinuousActionReference:
							break;
						case ActionTarget::TileTypeReference:
							break;
						case ActionTarget::Gamestate:
							break;
						}
					}

				}

				index++;

				if (ImGui::Button(actionInfo.c_str()))
				{
					assignment.assignActionOrReplace(action);
					break;
				}
			}
		}
		

		ImGui::EndGroup();

		ImGui::EndChild();
		ImGui::End();
	}

	void RTSGameRenderer::createWindowNavMesh()
	{
		// Disabled due to new Agent-System
		// ToDo - Enable GUI to modify the game
		
		//ImGui::Begin("Debug Navigation");
		//if (ImGui::Button("Generate NavMesh"))
		//{
		//	game->navigationConfig = config;
		//	game->shouldUpdateNavmesh = true;
		//}
		//
		//// TODO add more precision to the navmesh generation
		///*ImGui::Text("Rasterization");
		//ImGui::SliderFloat("Cell Size", &config.m_cellSize, 0.1f, 1.0f);
		//ImGui::SliderFloat("Cell Height", &config.m_cellHeight, 0.1f, 1.0f);*/
		//
		//ImGui::Separator();
		//ImGui::Text("Agent");
		////ImGui::SliderFloat("Height", &config.m_agentHeight, 0.1f, 5.0f);
		//ImGui::SliderFloat("Radius", &config.m_agentRadius, 0.0f, 5.0f);
		///*ImGui::SliderFloat("Max Climb", &config.m_agentMaxClimb, 0.1f, 5.0f);
		//ImGui::SliderFloat("Max Slope", &config.m_agentMaxSlope, 0.0f, 90.0f);*/
		//
		//ImGui::Separator();
		//ImGui::Text("Region");
		//ImGui::SliderFloat("Min Region Size", &config.m_regionMinSize, 0.0f, 150.0f);
		//ImGui::SliderFloat("Merged Region Size", &config.m_regionMergeSize, 0.0f, 150.0f);
		//
		//ImGui::Separator();
		//ImGui::Text("Erode mesh");
		//ImGui::Checkbox("Erode Walkable Area", &config.m_erodeWalkableArea);
		//
		//ImGui::Separator();
		//ImGui::Text("Partitioning");
		//ImGui::RadioButton("Watershed", &config.m_partitionType, SAMPLE_PARTITION_WATERSHED);
		//ImGui::RadioButton("Monotone", &config.m_partitionType, SAMPLE_PARTITION_MONOTONE);
		//ImGui::RadioButton("Layers", &config.m_partitionType, SAMPLE_PARTITION_LAYERS);
		//
		//ImGui::Separator();
		//ImGui::Text("Filtering");
		//ImGui::Checkbox("Low Hanging Obstacles", &config.m_filterLowHangingObstacles);
		//ImGui::Checkbox("Ledge Spans", &config.m_filterLedgeSpans);
		//ImGui::Checkbox("Walkable Low Height Spans", &config.m_filterWalkableLowHeightSpans);
		//
		//ImGui::Separator();
		//ImGui::Text("Polygonization");
		//ImGui::SliderFloat("Max Edge Length", &config.m_edgeMaxLen, 0.0f, 50.0f);
		//ImGui::SliderFloat("Max Edge Error", &config.m_edgeMaxError, 0.1f, 3.0f);
		//ImGui::SliderFloat("Verts Per Poly", &config.m_vertsPerPoly, 3.0f, 12.0f);
		//
		//ImGui::Separator();
		//ImGui::Text("Detail Mesh");
		//ImGui::SliderFloat("Sample Distance", &config.m_detailSampleDist, 0.0f, 16.0f);
		//ImGui::SliderFloat("Max Sample Error", &config.m_detailSampleMaxError, 0.0f, 16.0f);
		//
		//ImGui::End();
	}

	void RTSGameRenderer::createWindowFogOfWar()
	{
		ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);

		ImGui::Begin("Fog of War window");

		if (config->applyFogOfWar)
			ImGui::Text("Is enabled in config");
		else
			ImGui::Text("Is disabled in config");

		if (Widgets::fowController(state, fowSettings))
		{
			// Selected player changed -> Re-Apply FogOfWar
			fowState = state;
			fowState.applyFogOfWar(fowSettings.selectedPlayerID);
		}

		ImGui::End();
	}

	void RTSGameRenderer::createWindowPlayerParameters() const
	{
		ImGui::SetNextWindowSize(ImVec2(100, 150), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_FirstUseEver);
		ImGui::Begin("PlayerParameters");
		ImGui::BeginChild("Scrolling");
		ImGui::BeginGroup();

		if (pointOfViewPlayerID != NO_PLAYER_ID)
		{
			const auto* player = state.getPlayer(fowSettings.selectedPlayerID);
			for (const auto& parameter : state.getGameInfo()->getPlayerParameterTypes())
			{
				//Double to string with 2 precision				
				std::stringstream stream;
				stream << std::fixed << std::setprecision(2) << player->getRawParameterAt(parameter.second.getIndex());
				std::string valueParameter = stream.str();

				std::string parameterInfo = parameter.second.getName() + ": " + valueParameter;
				ImGui::BulletText(parameterInfo.c_str());
			}
		}

		ImGui::EndGroup();

		ImGui::EndChild();
		ImGui::End();
	}

	void RTSGameRenderer::setPlayerPointOfView(int playerID)
	{
		pointOfViewPlayerID = playerID;
		fowSettings.selectedPlayerID = playerID;
		update(state);
	}

	ActionAssignment RTSGameRenderer::getPlayerActions()
	{
		return assignment;
	}

	bool RTSGameRenderer::isGameEndRequested()
	{
		return endGameRequested;
	}

}