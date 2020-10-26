#include <TBSGameStateRender.h>
#include <imgui-SFML.h>
#include <imgui.h>

TBSGameStateRender::TBSGameStateRender(SGA::TBSGame& game, const std::unordered_map<int, std::string>& tileSprites, const std::unordered_map<int, std::string>& unitSprites, int playerID) :
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

void TBSGameStateRender::init()
{
	GameStateRenderer::init();
	
	//Initialization
	initializeLayers();
}

void TBSGameStateRender::onGameStateAdvanced()
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

	if (actionsStatsPerPlayerTurn.size() >= gameStateCopy.currentGameTurn) {
		actionsStatsPerPlayerTurn.resize(actionsStatsPerPlayerTurn.size() + 100);
	}

	actionsStatsPerPlayerTurn[gameStateCopy.currentGameTurn].resize(gameStateCopy.getPlayers().size());
	//Add profiling info
	for (size_t i = 0; i < gameStateCopy.getPlayers().size(); i++)
	{

		actionsStatsPerPlayerTurn[gameStateCopy.currentGameTurn][i].add(game->getForwardModel().getActions(gameStateCopy, i)->count());

	}
}

void TBSGameStateRender::init(const std::unordered_map<int, std::string>& tileSprites, const std::unordered_map<int, std::string>& unitSprites)
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

void TBSGameStateRender::run(bool& isRunning)
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

void TBSGameStateRender::handleInput(sf::RenderWindow& window)
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
		if (selectedUnit)
		{
			//Recollect each action in tile
			std::vector<SGA::Action<SGA::Vector2i>> actionsInTile;
			for (const auto& action : actionHumanUnitSelected)
			{
				if (action.getTargetPosition() == SGA::Vector2i(pos.x, pos.y))
				{
					actionsInTile.emplace_back(action);
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
			selectedUnit = nullptr;
		}

		SGA::TBSUnit* unit = gameStateCopy.getUnit(SGA::Vector2i(pos.x, pos.y));

		if (unit)
		{

			//Assign selected unit			
			selectedUnit = unit;
			if (unit->getPlayerID() == gameStateCopy.currentPlayer)
			{
				actionHumanUnitSelected.clear();
				for (const auto& action : *actionsHumanCanPlay)
				{
					//If is player unit action or globlal action(i.e End turn)
					if (action.getSourceUnitID() == unit->getUnitID() || (action.getSourceUnitID() == -1 && unit->getPlayerID() == gameStateCopy.currentPlayer))
						actionHumanUnitSelected.addAction(action);
				}
			}
		}
		else
		{
			//Restart selected actions of unit and selected unit
			actionHumanUnitSelected.clear();
			selectedUnit = nullptr;

			moving = true;
			oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
		}
	}
	if (event.mouseButton.button == sf::Mouse::Right)
	{
		if (waitForAction)
			playAction(SGA::Action<SGA::Vector2i>(SGA::ActionType::EndTurn, gameStateCopy.currentPlayer));
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

void TBSGameStateRender::initializeLayers()
{
	//Init Map Layer
	renderLayers.emplace_back(std::make_unique<MapLayer<SGA::TBSGameState>>(assetCache));
	renderLayers.back()->init(gameStateCopy);

	//Init Unit Layer
	renderLayers.emplace_back(std::make_unique<TBSUnitLayer>(assetCache));
	renderLayers.back()->init(gameStateCopy);

	//Init Unit Layer
	renderLayers.emplace_back(std::make_unique<TBSOverlayLayer>(assetCache, actionHumanUnitSelected, currentMousePos));
	renderLayers.back()->init(gameStateCopy);
}

void TBSGameStateRender::drawLayers(sf::RenderWindow& window)
{
	//Update and Draw each layer by order	
	if (!renderLayers.empty())
		for (auto& layer : renderLayers)
		{
			if (!drawGameStateBuffer)
			{
				//Draw current gameState
				layer->update(gameStateCopy);
				layer->draw(gameStateCopy, window);
			}
			else
			{
				layer->update(gameStatesBuffer.getElement(gameStatesBufferRCurrentIndex));
				layer->draw(gameStatesBuffer.getElement(gameStatesBufferRCurrentIndex), window);

			}

		}
}

void TBSGameStateRender::createHUD(sf::RenderWindow& window)
{
	createWindowInfo();
	createWindowUnits();
	createWindowActions();
	createWindowMultipleActions(window);
	createWindowProfiling();
}
void TBSGameStateRender::createWindowInfo() const
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
void TBSGameStateRender::createWindowUnits()
{
	ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(20, 150), ImGuiCond_FirstUseEver);
	ImGui::Begin("Units");

	ImGui::BeginChild("Scrolling");

	auto units = gameStateCopy.getUnits();

	for (auto& unit : units)
	{
		std::string unitInfo;
		unitInfo = "" + std::to_string(unit.getUnitID()) + " PID: " + std::to_string(unit.getPlayerID()) + " - Health: " + std::to_string(unit.getHealth());
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
	for (auto action : actionHumanUnitSelected)
	{
		std::string actionInfo = std::to_string(index) + " " + SGA::getActionType(action.getType());
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
void TBSGameStateRender::createWindowMultipleActions(sf::RenderWindow& window)
{

	if (showMultipleActions)
	{
		ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(window.mapCoordsToPixel(toISO(multipleActionsSourceTile.x, multipleActionsSourceTile.y)));
		ImGui::Begin("PlayAction");
		ImGui::BeginChild("Scrolling");
		ImGui::BeginGroup();

		int index = 0;
		for (auto action : actionHumanUnitSelected)
		{
			if (action.getTargetPosition() == multipleActionsSourceTile)
			{
				std::string actionInfo = std::to_string(index) + " " + SGA::getActionType(action.getType());
				index++;

				if (ImGui::Button(actionInfo.c_str()))
				{
					playAction(action);
					break;
				}
			}
		}

		ImGui::EndGroup();
		ImGui::EndChild();
		ImGui::End();
	}
}