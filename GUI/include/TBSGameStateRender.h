#pragma once
#include <GameStateRenderer.h>
#include <Game/TBSGame.h>
#include <Representation/TBSGameState.h>
#include <SFML/Window/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <CircularBuffer.h>


#include <StatsSummary.h>

class TBSGameStateRender: public GameStateRenderer<SGA::TBSGameState>
{
public:
	TBSGameStateRender(SGA::TBSGame& game, const std::unordered_map<int, std::string>& tileSprites, const std::unordered_map<int, std::string>& unitSprites, int playerID);
	void run(bool& isRunning) override;

	// GameCommunicator functions
	void onGameStateAdvanced() override;

private:
	void init() override;
	void init(const std::unordered_map<int, std::string>& tileSprites, const std::unordered_map<int, std::string>& unitSprites);
	void initializeView(sf::RenderWindow& window) const;
	void initializeLayers();

	void handleInput(sf::RenderWindow& window);

	//Events Implementation
	void windowClosed(const sf::Event& event, sf::View& view, sf::RenderWindow& window)const;
	void mouseScrolled(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
	void mouseButtonReleased(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
	void mouseButtonPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
	void mouseMoved(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
	void keyPressed(const sf::Event& event, sf::View& view, sf::RenderWindow& window);

	//Render Layers by order
	void drawLayers(sf::RenderWindow& window);

	//HUD
	void createHUD(sf::RenderWindow& window);

	void createWindowInfo()const;
	void createWindowUnits();
	void createWindowActions();
	void createWindowMultipleActions(sf::RenderWindow& window);
	void createWindowProfiling()
	{
		float* arr;
		int size = 200;
		arr = new float[size];
		memset(arr, 0, size * sizeof(float));
		ImGui::Begin("Action Space");

		if (ImGui::TreeNode("Graphs"))
		{
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("Graphs", tab_bar_flags))
			{
				if (ImGui::BeginTabItem("Sum"))
				{

					for (size_t i = 0; i < gameStateCopy.getPlayers().size(); i++)
					{
						for (size_t j = 0; j < size; j++)
						{


							if (!actionsStatsPerPlayerTurn[j].empty())
								arr[j] = (float)actionsStatsPerPlayerTurn[j][i].getSum();
							else
								arr[j] = 0;
						}

						std::string infoText = "Player: " + std::to_string(i);
						ImGui::Text(infoText.c_str());

						ImGui::PlotLines("", arr, size);
						//ImGui::PlotHistogram("Histogram", arr, size, 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));
					}
					ImGui::EndTabItem();

				}

				if (ImGui::BeginTabItem("Mean"))
				{

					memset(arr, 0, size * sizeof(float));
					//for (size_t i = 0; i < gameStateCopy.getPlayers().size(); i++)
					//{
					//	for (size_t j = 0; j < size; j++)
					//	{
					//		actionsStatsPerPlayerTurn[j][i].computeStats();
					//		if (!actionsStatsPerPlayerTurn[j].empty())
					//			arr[j] = (float)actionsStatsPerPlayerTurn[j][i].getMean();
					//		else
					//			arr[j] = 0;
					//	}

					//	std::string infoText = "Player: " + std::to_string(i);
					//	ImGui::Text(infoText.c_str());

					//	ImGui::PlotLines("", arr, size);
					//	//ImGui::PlotHistogram("Histogram", arr, size, 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));
					//}
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Max"))
				{

					memset(arr, 0, size * sizeof(float));
					for (size_t i = 0; i < gameStateCopy.getPlayers().size(); i++)
					{
						for (size_t j = 0; j < size; j++)
						{
							if (!actionsStatsPerPlayerTurn[j].empty())
								arr[j] = (float)actionsStatsPerPlayerTurn[j][i].getMax();
							else
								arr[j] = 0;
						}

						std::string infoText = "Player: " + std::to_string(i);
						ImGui::Text(infoText.c_str());

						ImGui::PlotLines("", arr, size);
						//ImGui::PlotHistogram("Histogram", arr, size, 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));
					}
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Min"))
				{

					memset(arr, 0, size * sizeof(float));
					for (size_t i = 0; i < gameStateCopy.getPlayers().size(); i++)
					{
						for (size_t j = 0; j < size; j++)
						{
							if (!actionsStatsPerPlayerTurn[j].empty())
								arr[j] = (float)actionsStatsPerPlayerTurn[j][i].getMin();
							else
								arr[j] = 0;
						}

						std::string infoText = "Player: " + std::to_string(i);
						ImGui::Text(infoText.c_str());

						ImGui::PlotLines("", arr, size);
						//ImGui::PlotHistogram("Histogram", arr, size, 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));
					}
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Multiplication"))
				{
					memset(arr, 0, size * sizeof(float));

					for (size_t i = 0; i < gameStateCopy.getPlayers().size(); i++)
					{
						for (size_t j = 0; j < size; j++)
						{
							if (!actionsStatsPerPlayerTurn[j].empty())
								arr[j] = (float)actionsStatsPerPlayerTurn[j][i].getMulti();
							else
								arr[j] = 0;
						}

						std::string infoText = "Player: " + std::to_string(i);
						ImGui::Text(infoText.c_str());

						ImGui::PlotLines("", arr, size);
						//ImGui::PlotHistogram("Histogram", arr, size, 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));
					}

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::Separator();
			ImGui::TreePop();
		}

		ImGui::End();
	}
	//Human player stuff
	void waitForHumanToPlay()
	{
		waitForAction = true;

		actionsHumanCanPlay = game->getForwardModel().getActions(gameStateCopy);
		actionHumanUnitSelected.clear();
	}

	void playAction(SGA::Action<SGA::Vector2i> action)
	{
		waitForAction = false;
		actionsHumanCanPlay->clear();
		actionHumanUnitSelected.clear();
		selectedUnit = nullptr;
		showMultipleActions = false;
		game->addActionToExecute(action);
	}

private:	

	//Game Data
	SGA::TBSGame* game;

	//Current gamestate used to render
	SGA::TBSGameState gameStateCopy;

	//Zoom
	float zoomValue = 5;

	//Mouse Information
	SGA::Vector2f currentMousePos;
	sf::Vector2f oldPos;
	bool moving = false;

	//FPS
	int fpsLimit = 60;
	int m_fps = 0;

	//Drawing gameState Buffer
	bool drawGameStateBuffer = false;
	CircularBuffer<SGA::TBSGameState> gameStatesBuffer;
	int gameStatesBufferRCurrentIndex = 0;

	//Human player
	bool waitForAction = false;
	std::unique_ptr<SGA::ActionSpace<SGA::Vector2i>> actionsHumanCanPlay;

	SGA::ActionSpace<SGA::Vector2i> actionHumanUnitSelected;
	SGA::TBSUnit* selectedUnit{};

	//Imgui
	sf::Clock deltaClock;

	//If a tile has more than one action, we render a window
	//to show all the action that can be played in that tile
	bool showMultipleActions = false;
	SGA::Vector2i multipleActionsSourceTile;

	//Profiling
	//Last call OnAdvancedGameGameState
	std::vector<std::vector<StatSummary>> actionsStatsPerPlayerTurn;
};
