#include <AbstractRTSGameStateRender.h>
#include <ForwardModel/AbstractFM/Action.h>
#include <CircularBuffer.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iomanip>
#include <sstream>

AbstractRTSGameStateRender::AbstractRTSGameStateRender(SGA::AbstractRTSGame& game, const std::unordered_map<int, std::string>& tileSprites, const std::unordered_map<int, std::string>& unitSprites, int playerID) :
	GameStateRenderer{ playerID },
	game(&game),
	gameStateCopy(game.getStateCopy()),
	gameStatesBuffer(50)
{
	init(tileSprites, unitSprites);
}

void AbstractRTSGameStateRender::init()
{
	GameStateRenderer::init();

	//Initialization
	initializeLayers();
}

void AbstractRTSGameStateRender::onGameStateAdvanced()
{
	std::lock_guard<std::mutex> lockGuard(mutexRender);
	gameStateCopy = game->getStateCopy();

	//Add state to buffer
	gameStatesBuffer.add(gameStateCopy);
	gameStatesBufferRCurrentIndex = gameStatesBuffer.getFront();
}

void AbstractRTSGameStateRender::init(const std::unordered_map<int, std::string>& tileSprites, const std::unordered_map<int, std::string>& unitSprites)
{
	//Need to activate the context before adding new textures
	ctx.setActive(true);

	//Load textures
	for (auto idPathPair : tileSprites)
	{
		assetCache.loadTexture("tile_" + std::to_string(idPathPair.first), idPathPair.second);
	}

	for (const auto& idPathPair : unitSprites)
	{
		assetCache.loadTexture("unit_" + std::to_string(idPathPair.first), idPathPair.second);
	}

	// TODO Depends on location of configuration file, how to prevent that?
	assetCache.loadTexture("circleCollider", "../GUI/Assets/circleColliderFixed.png");
	assetCache.loadTexture("boxCollider", "../GUI/Assets/boxColliderFixed.png");
	assetCache.loadFont("font", "../GUI/Assets/arial.ttf");

	assetCache.loadTexture("building", "../GUI/Assets/building.png");;
}

void AbstractRTSGameStateRender::initializeView(sf::RenderWindow& window) const
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

void AbstractRTSGameStateRender::run(bool& isRunning)
{
	sf::RenderWindow window(sf::VideoMode(1200, 800), "Strategy Game Engine", sf::Style::Default | sf::Style::Titlebar);
	window.setActive(true);

	ImGui::SFML::Init(window);

	initializeView(window);

	//FPS stuff
	m_fps = 0;
	int m_fpscount = 0;
	sf::Clock clock;

	while (window.isOpen() && isRunning)
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

void AbstractRTSGameStateRender::handleInput(sf::RenderWindow& window)
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

void AbstractRTSGameStateRender::windowClosed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)const
{
	window.close();
	game->end();
}
void AbstractRTSGameStateRender::mouseScrolled(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
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
void AbstractRTSGameStateRender::mouseButtonReleased(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
{
	// Mouse button is released, no longer move
	if (event.mouseButton.button == sf::Mouse::Middle) {
		moving = false;
	}

	if (event.mouseButton.button == sf::Mouse::Left)
	{
		dragging = false;

		sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		auto worldPos = toGridFloat(pos);
		auto movementDir = pos - oldPos;
		auto movementDistance = std::sqrt(movementDir.x * movementDir.x + movementDir.y * movementDir.y);

		if (movementDistance <= 10.0)
		{
			// The user clicked somewhere
			if (!selectedUnits.empty())
			{
				auto* unit = gameStateCopy.getEntity(SGA::Vector2f(worldPos.x, worldPos.y), 1);

				if (unit)
				{
					for (const auto& i : selectedUnits)
					{
						
						//game->executeAction(SGA::RTSAction(SGA::RTSActionType::Attack, getPlayerID(), i, unit->position, unit->unitID));
					}
				}
				else
				{
					for (const auto& i : selectedUnits)
					{
						auto entityType = gameStateCopy.getEntityType(gameStateCopy.getEntity(i).typeID);
						
						//Move action
						if(entityType.haveActionType(2))
						{
							SGA::Action newAction;
							newAction.actionTypeID = 2;
							newAction.targets.emplace_back(i);
							//SGA::ActionTarget targetPos=SGA::ActionTarget(SGA::Vector2f(worldPos.x, worldPos.y));
							newAction.targets.emplace_back(SGA::Vector2f(worldPos.x, worldPos.y));
							newAction.owner = getPlayerID();
							game->executeAction(newAction);
							
						}
					}
				}

				selectedUnits.clear();
			}
			else
			{
				selectedUnits.clear();
				/*auto* unit = gameStateCopy.getUnit(SGA::Vector2f(worldPos.x, worldPos.y));
				if (unit != nullptr && unit->playerID == getPlayerID())
					selectedUnits.emplace(unit->unitID);*/
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
					if (unit.owner == getPlayerID())
						selectedUnits.emplace(unit.id);
				}
			}
		}
	}
}
void AbstractRTSGameStateRender::mouseButtonPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
{
	// Mouse button is pressed, get the position and set moving as active
	if (event.mouseButton.button == sf::Mouse::Left)
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
void AbstractRTSGameStateRender::mouseMoved(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
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
void AbstractRTSGameStateRender::keyPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
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

void AbstractRTSGameStateRender::initializeLayers()
{
	////Init Map Layer
	//renderLayers.emplace_back(std::make_unique<MapLayer<SGA::RTSGameState>>(assetCache));
	//renderLayers.back()->init(gameStateCopy);

	//////Init Unit Layer
	//renderLayers.emplace_back(std::make_unique<RTSUnitLayer>(assetCache, *this));
	//renderLayers.back()->init(gameStateCopy);

	////Init Unit Layer
	//renderLayers.emplace_back(std::make_unique<RTSOverlayLayer>(assetCache, *this, currentMousePos));
	//renderLayers.back()->init(gameStateCopy);
}

void AbstractRTSGameStateRender::drawLayers(sf::RenderWindow& window)
{
	////Update and Draw each layer by order	
	//if (!renderLayers.empty())
	//	for (auto& layer : renderLayers)
	//	{
	//		if (!drawGameStateBuffer)
	//		{
	//			//Draw current gameState
	//			layer->update(gameStateCopy);
	//			layer->draw(gameStateCopy, window);
	//		}
	//		else
	//		{
	//			layer->update(gameStatesBuffer.getElement(gameStatesBufferRCurrentIndex));
	//			layer->draw(gameStatesBuffer.getElement(gameStatesBufferRCurrentIndex), window);

	//		}

	//	}

	mapSprites.clear();
	entitySprites.clear();
	unitsInfo.clear();
	healthBars.clear();
	
	SGA::Board& board = gameStateCopy.board;

	for (int y = 0; y < board.getHeight(); ++y)
	{
		for (int x = 0; x < board.getWidth(); ++x)
		{
			auto& targetTile = board.getTile(x, y);
			sf::Texture& texture = assetCache.getTexture("tile_" + std::to_string(targetTile.tileTypeID));
			sf::Vector2f origin(TILE_ORIGIN_X, TILE_ORIGIN_Y);
			sf::Sprite newTile(texture);

			newTile.setPosition(toISO(x, y));
			newTile.setOrigin(origin);
			mapSprites.emplace_back(newTile);
		}
	}

	for (const auto& sprite : mapSprites)
	{
		window.draw(sprite);
	}

	//Draw entities
	for (auto& entity : gameStateCopy.entities)
	{
		//Check if entity have sprite
		auto searchedEntity = entitySpriteTypes->find(entity.typeID);
		if (searchedEntity != entitySpriteTypes->end())
		{
			//Add units
			sf::Texture& texture = assetCache.getTexture(searchedEntity->second);
			//sf::Vector2f origin(0, texture.getSize().y / 1.4);
			sf::Vector2f origin(TILE_ORIGIN_X, TILE_ORIGIN_Y);
			sf::Sprite newUnit(texture);

			sf::Vector2f pos = toISO(entity.position.x, entity.position.y);
			newUnit.setPosition(pos.x /*+ TILE_WIDTH_HALF / 2*/, pos.y /*+ TILE_HEIGHT_HALF / 2*/);

			newUnit.setOrigin(origin);
			entitySprites.emplace_back(newUnit);

			//Add units text info
			sf::Text unitInfo;
			unitInfo.setFont(assetCache.getFont("font"));
			std::string info = "PlayerID: " + std::to_string(entity.owner) + " ID: " + std::to_string(entity.id);
			/*const auto& entityType=gameStateCopy.getEntityType(entity.typeID);*/
			for (size_t i = 0; i < entity.parameters.size(); i++)
			{

				// Create an output string stream
				std::ostringstream streamObj3;
				// Set Fixed -Point Notation
				streamObj3 << std::fixed;
				// Set precision to 2 digits
				streamObj3 << std::setprecision(2);
				streamObj3 << entity.parameters[i];

				info += "/" + streamObj3.str();
			}
			unitInfo.setString(info);
			unitInfo.setPosition(toISO(entity.position.x, entity.position.y));
			entityInfo.emplace_back(unitInfo);

			//Check if entity have health
			if(gameStateCopy.checkEntityHaveParameter(entity.typeID, "Health"))
			{
				int globalHealthID = gameStateCopy.getParameterGlobalID("Health");

				double& health = gameStateCopy.getParameterReference(entity.id, globalHealthID);
				double maxHealth = gameStateCopy.getParameterType(entity.typeID, globalHealthID).maxValue;
				
				//Add temporal Health bar
				sf::RectangleShape background;
				sf::Vector2f backgroundSize(140, 35);
				sf::Vector2f fillSize(130, 25);

				int yOffset = -220;

				background.setPosition(pos.x + TILE_WIDTH_HALF, pos.y + yOffset);
				background.setFillColor(sf::Color::Black);
				background.setSize(backgroundSize);
				background.setOrigin(backgroundSize.x / 2, backgroundSize.y / 2);
				healthBars.emplace_back(background);

				sf::RectangleShape fill;
				fill.setPosition(pos.x + TILE_WIDTH_HALF, pos.y + yOffset);
				fill.setFillColor(sf::Color::Red);
				//Compute fill percentage
				float percentage = (float)health / (float)maxHealth;
				fill.setSize(sf::Vector2f(fillSize.x * percentage, fillSize.y));
				fill.setOrigin(fillSize.x / 2, fillSize.y / 2);
				healthBars.emplace_back(fill);
			}
			
		}

		
	}	

	for (const auto& sprite : entitySprites)
	{
		window.draw(sprite);
	}

	for (const auto& sprite : healthBars)
	{
		window.draw(sprite);
	}

	//Check if units are selected
	for (const auto& unit : gameStateCopy.entities)
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
		const dtNavMesh* mesh = gameStateCopy.navigation->m_navMesh;

		if (mesh)
		{
			//Draw navmesh polygons
			for (int i = 0; i < gameStateCopy.navigation->m_navMesh->getMaxTiles(); ++i)
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
	for (auto& unit : gameStateCopy.entities)
	{
		if (unit.executingAction.targets.size()>0)
			if (true/*unit.executingAction.type == SGA::RTSActionType::Move || unit.executingAction.type == SGA::RTSActionType::Attack*/)
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
	}
}

void AbstractRTSGameStateRender::createHUD(sf::RenderWindow& window)
{
	createWindowInfo();
	createWindowUnits();
	createWindowNavMesh();
}

void AbstractRTSGameStateRender::createWindowInfo()
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

	ImGui::End();
}

void AbstractRTSGameStateRender::createWindowUnits()
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
		unitInfo = type.name + " " + std::to_string(unit.id) + " PID: " + std::to_string(unit.owner);
		ImGui::Text(unitInfo.c_str());
	}

	ImGui::EndChild();
	ImGui::End();
}

void AbstractRTSGameStateRender::createWindowNavMesh()
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