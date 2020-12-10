#include <AbstractGameStateRender.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iomanip>
#include <sstream>
AbstractGameStateRender::AbstractGameStateRender(SGA::AbstractTBSGame& game, const std::unordered_map<int, std::string>& tileSprites, const std::unordered_map<int, std::string>& unitSprites, int playerID) :
	GameStateRenderer{ playerID },
	game(&game),
	gameStateCopy(game.getState()),
	gameStatesBuffer(50)
{
	init(tileSprites, unitSprites);

	if (gameStateCopy.currentPlayer == getPlayerID())
	{
		std::cout << "Wait for GUI Action" << std::endl;
		waitForHumanToPlay();
	}
}

void AbstractGameStateRender::init()
{
	GameStateRenderer::init();

	//Initialization
	initializeLayers();
}

void AbstractGameStateRender::onGameStateAdvanced()
{
	std::lock_guard<std::mutex> lockGuard(mutexRender);
	gameStateCopy = game->getStateCopy();

	//Add state to buffer
	gameStatesBuffer.add(gameStateCopy);
	gameStatesBufferRCurrentIndex = gameStatesBuffer.getFront();
	if (gameStateCopy.currentPlayer == getPlayerID())
	{
		std::cout << "Wait for GUI Action" << std::endl;
		waitForHumanToPlay();
	}
}

void AbstractGameStateRender::init(const std::unordered_map<int, std::string>& tileSprites, const std::unordered_map<int, std::string>& unitSprites)
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
	assetCache.loadTexture("selected", "../GUI/Assets/selected.png");
	assetCache.loadFont("font", "../GUI/Assets/arial.ttf");

	
	assetCache.loadTexture("building", "../GUI/Assets/building.png");


}

void AbstractGameStateRender::initializeView(sf::RenderWindow& window) const
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

void AbstractGameStateRender::run(bool& isRunning)
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Strategy Game Engine", sf::Style::Default | sf::Style::Titlebar);
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
			ImGui::SFML::Update(window, deltaClock.restart());

			createHUD(window);
		}

		ImGui::SFML::Render(window);
		window.display();
	}
	game->end();
	window.close();
}

void AbstractGameStateRender::handleInput(sf::RenderWindow& window)
{
	//Get current View to edit it and apply changes
	sf::View view = window.getView();

	sf::Event event;
	while (window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		if (ImGui::IsAnyWindowHovered())
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

void AbstractGameStateRender::windowClosed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)const
{
	window.close();
	game->end();
}
void AbstractGameStateRender::mouseScrolled(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
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
void AbstractGameStateRender::mouseButtonReleased(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
{
	// Mouse button is released, no longer move
	if (event.mouseButton.button == sf::Mouse::Left) {
		moving = false;
	}
}
void AbstractGameStateRender::mouseButtonPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
{
	// Mouse button is pressed, get the position and set moving as active
	if (event.mouseButton.button == sf::Mouse::Left)
	{

		sf::Vector2i pos = toGrid(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));

		//If selected unit we check if there is action in tile
		if (selectedEntity)
		{
			//Recollect each action in tile
			std::vector<SGA::Action> actionsInTile;
			for (const auto& action : actionHumanUnitSelected)
			{
				SGA::ActionType& actionType = gameStateCopy.getActionType(action.actionTypeID);
				if (actionType.sourceType == SGA::ActionSourceType::Unit)
				{
					if(actionType.actionTargets.type==SGA::TargetType::Entity)
					{
						if (SGA::targetToEntity(gameStateCopy, action.targets[1]).position == SGA::Vector2i(pos.x, pos.y))
						{
							actionsInTile.emplace_back(action);
						}
					}
					else
					{
						if (SGA::targetToPosition(gameStateCopy, action.targets[1]) == SGA::Vector2i(pos.x, pos.y))
						{
							actionsInTile.emplace_back(action);
						}
					}
						
						
				}
			}

			//Check number of actions in tile
			if (actionsInTile.size() > 1)
			{
				//Open window to show actions
				showMultipleActions = true;
				multipleActionsSourceTile = SGA::Vector2i(pos.x, pos.y);
			}
			else if (!actionsInTile.empty())
			{
				//Play action directly
				playAction(actionsInTile[0]);
			}
			else
			{
				//If there is no action, hide window
				showMultipleActions = false;
			}

		}
		else
		{
			showMultipleActions = false;
			selectedEntity = nullptr;
		}

		SGA::Entity* unit = gameStateCopy.getEntity(SGA::Vector2f(pos.x, pos.y));

		if (unit)
		{

			//Assign selected unit			
			selectedEntity = unit;
			if (unit->owner == gameStateCopy.currentPlayer)
			{
				actionHumanUnitSelected.clear();
				for (const auto& action : actionsHumanCanPlay)
				{
					//If is player unit action or globlal action(i.e End turn)
					
					auto& actionType=gameStateCopy.getActionType(action.actionTypeID);
					
					if(actionType.actionTargets== SGA::TargetType::Entity)
					{
						auto& entity = SGA::targetToEntity(gameStateCopy, action.targets[0]);
						if (entity.id==unit->id/*action.sourceUnitID == unit->getUnitID() ||*/ /*(action.sourceUnitID == -1 &&*/ /*)*/)
							actionHumanUnitSelected.emplace_back(action);
					}
					else if(actionType.actionTargets == SGA::TargetType::Position)
					{
						if (SGA::targetToEntity(gameStateCopy, action.targets[0]).id==unit->id)
							actionHumanUnitSelected.emplace_back(action);
					}
					
				}
			}
		}
		else
		{
			//Restart selected actions of unit and selected unit
			actionHumanUnitSelected.clear();
		selectedEntity = nullptr;

			moving = true;
			oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
		}
	}
	if (event.mouseButton.button == sf::Mouse::Right)
	{
		if (waitForAction)
		{
			playAction(actionsHumanCanPlay[actionsHumanCanPlay.size()-1]);
		}
			
	}
}
void AbstractGameStateRender::mouseMoved(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
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
void AbstractGameStateRender::keyPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
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

void AbstractGameStateRender::initializeLayers()
{
	////Init Map Layer
	//renderLayers.emplace_back(std::make_unique<MapLayer<SGA::TBSGameState>>(assetCache));
	//renderLayers.back()->init(gameStateCopy);

	////Init Unit Layer
	//renderLayers.emplace_back(std::make_unique<TBSUnitLayer>(assetCache));
	//renderLayers.back()->init(gameStateCopy);

	////Init Unit Layer
	//renderLayers.emplace_back(std::make_unique<TBSOverlayLayer>(assetCache, actionHumanUnitSelected, currentMousePos));
	//renderLayers.back()->init(gameStateCopy);
}

void AbstractGameStateRender::drawLayers(sf::RenderWindow& window)
{
	//Update and Draw each layer by order	
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


	//Draw Board
	mapSprites.clear();
	entitySprites.clear();
	entityInfo.clear();
	overlaySprites.clear();
	actionsSelectedEntity.clear();
	
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

	//Draw selectedtile
	//Add actions if we have actions to draw
	if (actionHumanUnitSelected.size() > 0)
	{
		for (const auto& action : actionHumanUnitSelected)
		{
			SGA::ActionType& actionType = gameStateCopy.getActionType(action.actionTypeID);
			if(actionType.sourceType==SGA::ActionSourceType::Unit)
			{
				
				
				//Get source
				if(actionType.actionTargets.type==SGA::TargetType::Entity)
				{
					const SGA::Entity& targetEntity = SGA::targetToEntity(gameStateCopy, action.targets[1]);

					sf::CircleShape shape(15);
					sf::Vector2f temp = toISO(targetEntity.position.x, targetEntity.position.y);

					shape.setPosition(temp + sf::Vector2f(TILE_OFFSET_ORIGIN_X, TILE_OFFSET_ORIGIN_Y));
					actionsSelectedEntity.emplace_back(shape);
				}
				else
				{
					const SGA::Vector2f& targetPos = SGA::targetToPosition(gameStateCopy, action.targets[1]);

					sf::CircleShape shape(15);
					sf::Vector2f temp = toISO(targetPos.x, targetPos.y);

					shape.setPosition(temp + sf::Vector2f(TILE_OFFSET_ORIGIN_X, TILE_OFFSET_ORIGIN_Y));
					actionsSelectedEntity.emplace_back(shape);
					shape.setFillColor(sf::Color::Green);
				}
				
				
				/*switch (action.type)
				{
				case SGA::TBSActionType::Attack: shape.setFillColor(sf::Color::Red); break;
				case SGA::TBSActionType::Move:  break;
				case SGA::TBSActionType::Heal: shape.setFillColor(sf::Color::Green);  break;
				case SGA::TBSActionType::Push: shape.setFillColor(sf::Color::Black);  break;
				case SGA::TBSActionType::EndTurn:  shape.setFillColor(sf::Color::Magenta); break;
				default: throw std::runtime_error("Tried adding an action with an not supported action-type");
				}*/
				
			}
			
		}
	}

	for (const auto& sprite : actionsSelectedEntity)
	{
		window.draw(sprite);
	}
	
	//Add selected tile
	sf::Vector2i mouseGridPos = toGrid(sf::Vector2f(currentMousePos.x, currentMousePos.y));

	if (gameStateCopy.isInBounds(SGA::Vector2i(mouseGridPos.x, mouseGridPos.y)))
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
	for (auto& entity : gameStateCopy.entities)
	{
		//Check if entity have sprite
		auto searchedEntity = entitySpriteTypes->find(entity.typeID);
		if(searchedEntity!=entitySpriteTypes->end())
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
		}
	}

	for (const auto& sprite : entitySprites)
	{
		window.draw(sprite);
	}
	for (const auto& info : entityInfo)
	{
		window.draw(info);
	}
	
}

void AbstractGameStateRender::createHUD(sf::RenderWindow& window)
{
	createWindowInfo();
	createWindowUnits();
	createWindowActions();
	createWindowMultipleActions(window);
}
void AbstractGameStateRender::createWindowInfo() const
{
	ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
	ImGui::Begin("Info window");

	std::string gameTurns = "Turn:" + std::to_string(gameStateCopy.currentGameTurn) + " Player Turn: " + std::to_string(gameStateCopy.currentPlayer) + " FPS: " + std::to_string(m_fps);
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
void AbstractGameStateRender::createWindowUnits()
{
	ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(20, 150), ImGuiCond_FirstUseEver);
	ImGui::Begin("Units");

	ImGui::BeginChild("Scrolling");

	/*auto units = gameStateCopy.getUnits();

	for (auto& unit : units)
	{
		std::string unitInfo;
		unitInfo = "" + std::to_string(unit.getUnitID()) + " PID: " + std::to_string(unit.getPlayerID()) + " - Health: " + std::to_string(unit.getHealth());
		ImGui::Text(unitInfo.c_str());
	}*/

	ImGui::EndChild();
	ImGui::End();
}
void AbstractGameStateRender::createWindowActions()
{
	ImGui::SetNextWindowSize(ImVec2(200, 150), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(20, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin("Actions");
	ImGui::BeginChild("Scrolling");
	ImGui::BeginGroup();

	int index = 0;
	for (auto action : actionHumanUnitSelected)
	{
		std::string actionInfo = std::to_string(index) + " " /*+ SGA::getActionType(action.type)*/;
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
void AbstractGameStateRender::createWindowMultipleActions(sf::RenderWindow& window)
{

	if (showMultipleActions)
	{
		ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(window.mapCoordsToPixel(toISO(multipleActionsSourceTile.x, multipleActionsSourceTile.y)));
		ImGui::Begin("PlayAction");
		ImGui::BeginChild("Scrolling");
		ImGui::BeginGroup();

		/*int index = 0;
		for (auto action : actionHumanUnitSelected)
		{
			if (action.targetPosition == multipleActionsSourceTile)
			{
				std::string actionInfo = std::to_string(index) + " " + getActionType(action.type);
				index++;

				if (ImGui::Button(actionInfo.c_str()))
				{
					playAction(action);
					break;
				}
			}
		}*/

		ImGui::EndGroup();
		ImGui::EndChild();
		ImGui::End();
	}
}