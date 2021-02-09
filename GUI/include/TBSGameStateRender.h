#pragma once
#include <GameStateRenderer.h>
#include <Game/TBSGame.h>
#include <Representation/TBSGameState.h>
#include <SFML/Window/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <CircularBuffer.h>

#include <ForwardModel/Action.h>

namespace SGA
{
	class TBSGameStateRender : public GameStateRenderer<TBSGameState>
	{
	public:
		TBSGameStateRender(TBSGame& game, const std::unordered_map<int, std::string>& tileSprites, const std::map<std::string, std::string>& entitySpritePaths, int playerID);
		void render() override;

		// GameCommunicator functions
		void onGameStateAdvanced() override;

	private:
		void init() override;
		void init(const std::unordered_map<int, std::string>& tileSprites, const std::map<std::string, std::string>& entitySpritePaths);
		void initializeView(sf::RenderWindow& window) const;
		
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

		void createWindowInfo() const;
		void createWindowUnits();
		void createWindowActions();
		void createWindowPlayerParameters() const;
		void createWindowMultipleActions(sf::RenderWindow& window);
		void createWindowFogOfWar();
		void createTopBar() const;
		void createEntityInformation(sf::RenderWindow& window);
		void createActionBar(sf::RenderWindow& window);
		

		//Human player stuff
		void waitForHumanToPlay()
		{
			waitForAction = true;

			actionsHumanCanPlay = game->getForwardModel().generateActions(gameStateCopyFogOfWar, getPlayerID());
			actionHumanUnitSelected.clear();
		}

		void playAction(Action action)
		{
			waitForAction = false;
			actionsHumanCanPlay.clear();
			actionHumanUnitSelected.clear();
			showMultipleActions = false;
			
			game->addActionToExecute(action);
		}

		void updatePossibleActions()
		{
			actionHumanUnitSelected.clear();
			
			if (selectedActionType == -1)
				return;
			
			//Check if we have selected a entity
			//if not, we take the player actions
			if(selectedEntityID!=-1)
			{
				auto* selectedEntity = gameStateCopy.getEntity(selectedEntityID);
				if (selectedEntity->ownerID == gameStateCopy.currentPlayer)
				{					
					for (const auto& action : actionsHumanCanPlay)
					{
						//If is player unit action or globlal action(i.e End turn)

						//SpecialActions like EndTurn and AbortContinunousAction
						if (action.actionTypeID == -1)
						{
							if (action.actionTypeFlags == AbortContinuousAction)
							{
								if (action.targets[0].getType() == ActionTarget::EntityReference)
									if (action.targets[0].getEntity(gameStateCopy).id == selectedEntity->id)
										actionHumanUnitSelected.emplace_back(action);
							}
							continue;
						}
							
													
						//Other actions
						auto& actionType = gameStateCopy.getActionType(action.actionTypeID);
						
						if (actionType.id != selectedActionType)
							continue;
						if(actionType.sourceType==ActionSourceType::Unit)
						{
							if(action.targets[0].getEntityID()==selectedEntityID)
							actionHumanUnitSelected.emplace_back(action);
						}
						/*else if (actionType.actionTargets == TargetType::ContinuousAction)
						{
							if (action.targets[0].getEntity(gameStateCopy).id == selectedEntity->id)
								actionHumanUnitSelected.emplace_back(action);
						}*/
					}
				}
			}
			else
			{
				for (const auto& action : actionsHumanCanPlay)
				{
					//SpecialActions like EndTurn and AbortContinunousAction
					if (action.actionTypeID == -1)
					{
						if (action.actionTypeFlags == AbortContinuousAction)
						{
							if (action.targets[0].getType() == ActionTarget::PlayerReference)
								actionHumanUnitSelected.emplace_back(action);
						}
						continue;
					}
										
					if (action.actionTypeID != selectedActionType)
						continue;
					
					auto& actionType = gameStateCopy.getActionType(action.actionTypeID);										
					if (actionType.sourceType == ActionSourceType::Player)
					{
						actionHumanUnitSelected.emplace_back(action);
					}		
				}
			}
		}
	private:

		//Game Data
		TBSGame* game;

		//Current gamestate used to render
		TBSGameState gameStateCopy;
		TBSGameState gameStateCopyFogOfWar;

		//Zoom
		float zoomValue = 5;

		//Mouse Information
		Vector2f currentMousePos;
		sf::Vector2f oldPos;
		bool moving = false;

		//FPS
		int fpsLimit = 60;
		int m_fps = 0;

		//Drawing gameState Buffer
		bool drawGameStateBuffer = false;
		CircularBuffer<TBSGameState> gameStatesBuffer;
		int gameStatesBufferRCurrentIndex = 0;

		//Human player
		bool waitForAction = false;
		std::vector<Action> actionsHumanCanPlay;

		std::vector<Action> actionHumanUnitSelected;
		int selectedEntityID=-1;

		//Imgui
		sf::Clock deltaClock;

		//If a tile has more than one action, we render a window
		//to show all the action that can be played in that tile
		bool showMultipleActions = false;
		Vector2i multipleActionsSourceTile;

		//Temp
		int selectedActionType = -1;
	};

}