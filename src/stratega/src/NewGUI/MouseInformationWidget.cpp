#include <Stratega/NewGUI/MouseInformationWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>     // std::cout, std::fixed
#include <iomanip> 
#include <sstream>
namespace SGA
{
	MouseInformationWidget::MouseInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFm):
		SGAWidget(widgetName, newWindow, newWorld, newFm)
	{
	}

	void MouseInformationWidget::update(const GameState& /*state*/)
	{
		
	}
	
	void MouseInformationWidget::render(SGARenderTarget& /*renderTarget*/)
	{
		auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		auto gridPos = world.toStratega(mousePos);

		if (drawGridTile)
		{
			//Draw mouse tile
			sf::VertexArray selectedTile(sf::Lines, 8);
			
			auto gridPosI = SGA::Vector2i(static_cast<int>(std::floor(gridPos.x)), static_cast<int>(std::floor(gridPos.y)));
			selectedTile[0] = sf::Vertex(world.toSFML(gridPosI), sf::Color::Yellow);
			selectedTile[1] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y)), sf::Color::Yellow);

			selectedTile[2] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y)), sf::Color::Yellow);
			selectedTile[3] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y + 1)), sf::Color::Yellow);

			selectedTile[4] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y + 1)), sf::Color::Yellow);
			selectedTile[5] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x, gridPosI.y + 1)), sf::Color::Yellow);

			selectedTile[6] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x, gridPosI.y + 1)), sf::Color::Yellow);
			selectedTile[7] = sf::Vertex(world.toSFML(Vector2i(gridPosI.x, gridPosI.y)), sf::Color::Yellow);

			window.draw(selectedTile);
		}		

		ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 70), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
		ImGui::Begin("Mouse info");
		std::string gridPosText = "Grid Position: (" + std::to_string(gridPos.x) + "," + std::to_string(gridPos.y) + ")";
		auto strategaMousPos = world.toStrategaRounded(mousePos);
		std::string gridMousePosText = "Grid Position Round: (" + std::to_string(strategaMousPos.x) + "," + std::to_string(strategaMousPos.y) + ")";
		std::string windowMousePosText = "Mouse Position Round: (" + std::to_string((int)mousePos.x) + "," + std::to_string((int)mousePos.y) + ")";
		ImGui::Text("%s", gridMousePosText.c_str());
		ImGui::Text("%s", windowMousePosText.c_str());
		ImGui::Checkbox("Draw mouse over grid", &drawGridTile);
		ImGui::End();
	}

	GridInformationWidget::GridInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFm, std::unordered_set<int>& newSelectedEntities) :
		SGAWidget(widgetName, newWindow, newWorld, newFm),
		currentGameState(nullptr),
		selectedEntities(newSelectedEntities)
	{
	}

	void GridInformationWidget::update(const GameState& state)
	{
		currentGameState = &state;
	}

	void GridInformationWidget::render(SGARenderTarget& renderTarget)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 90), ImGuiCond_FirstUseEver);

		ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
		ImGui::Begin("Grid information");
		ImGui::Checkbox("Draw entity information", &drawEntityInformation);
		ImGui::Checkbox("Draw tile information", &drawTileInformation);
		ImGui::End();
		//ImGuiIO& io = ImGui::GetIO();
		/*if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered() || io.WantCaptureMouse)
			return;
		else*/
		{

			auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			auto gridPos = world.toStratega(mousePos);
			auto* entity = currentGameState->getEntityAtConst(gridPos);
			
			//Check if we have entities selected
			if (selectedEntities.size() > 0)
			{
				entity = currentGameState->getEntityConst(*selectedEntities.begin());
			}
				
			if (entity)
			{
				if (!drawEntityInformation)
					return;

				ImGuiWindowFlags window_flags = 0;
				//window_flags += ImGuiWindowFlags_NoTitleBar;
				//window_flags += ImGuiWindowFlags_NoScrollbar;
				//window_flags += ImGuiWindowFlags_MenuBar;
				//window_flags+= ImGuiWindowFlags_NoMove;
				//window_flags += ImGuiWindowFlags_NoResize;
				//window_flags += ImGuiWindowFlags_NoCollapse;
				//window_flags += ImGuiWindowFlags_NoNav;
				////window_flags+= ImGuiWindowFlags_NoBackground;
				//window_flags += ImGuiWindowFlags_NoBringToFrontOnFocus;
				
				//Highlight entity drwable
				if(world.getEntity(entity->getID()))
					world.getEntity(entity->getID())->isHighlighted=true;

				ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);
				ImGui::SetNextWindowPos(ImVec2(0, static_cast<float>(window.getSize().y)), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
				ImGui::Begin("Entity Information", NULL, window_flags);

				auto& entityType = entity->getEntityType();

				ImGui::Text("%s", entityType.getName().c_str());
				ImGui::Columns(2, "mixed");
				ImGui::SetColumnWidth(0, 100.0f);

				ImGui::Separator();

				//Add units
				
				auto texture = renderTarget.getResourceManager().getEntitySprite(entityType).createSprite();

				ImGui::Image(
					texture,
					sf::Vector2f(100, 100),
					sf::Color::White,
					sf::Color::Transparent);


				ImGui::NextColumn();

				std::string owner = "Owner ID: ";
				owner += std::to_string(entity->getOwnerID());
				ImGui::Text("%s", owner.c_str());
				std::string iventorySize = "Iventory: ";
				iventorySize += std::to_string(entity->getInventoryUse()) +"/"+ std::to_string(entity->getInventorySize());
				ImGui::Text("%s", iventorySize.c_str());
				ImGui::Text("Parameters: ");

				for (const auto& parameter : entityType.getParameters())
				{
					//Double to string with 2 precision
					std::stringstream stream;
					stream << std::fixed << std::setprecision(2) << entity->getParameter(parameter.second.getName());
					std::string valueParameter = stream.str();

					std::string parameterInfo = parameter.second.getName() + ": " + valueParameter;
					ImGui::BulletText("%s", parameterInfo.c_str());
				}

				ImGui::NextColumn();

				ImGui::Columns(1);
				ImGui::Separator();

				ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;
				if (entity->getInventory().size())
				{
					ImGui::Text("Inventory: ");
					ImGui::BeginChild("Inventory", ImVec2(0, 80), true, child_flags);

					if (entity->getOwnerID() != -1)
						for (const auto& object : entity->getInventory())
						{
							//Check if entity have sprite
							auto searchedEntityType = object.getEntityType();
							//Add units
							if (ImGui::Button(searchedEntityType.getName().c_str(), { 0, 50 }))
							{

							}
							ImGui::SameLine();
						}

					ImGui::EndChild();
					ImGui::SameLine();
				}
				

				ImGui::Spacing();
				if (entity->getEntityType().getSlots().size() > 0)
				{
					ImGui::Text("Slots: ");
					ImGui::BeginChild("Slots", ImVec2(0, 60), true, child_flags);
					std::string tempText = "";
					int currenSlotId = 0;
					if (entity->getOwnerID() != -1)
						for (const auto& slot : entity->getEntityType().getSlots())
						{
							tempText = slot;
							const auto* slotUsedObject = entity->getSlotObjectBySlotId(currenSlotId);

							if (slotUsedObject)
							{
								tempText += "\n ";
								tempText += slotUsedObject->getEntityType().getName();
							}
								
							if (ImGui::Button(tempText.c_str(), { 0,50 }))
							{

							}
							ImGui::SameLine();
							currenSlotId++;
						}
					ImGui::EndChild();
					ImGui::SameLine();
				}				
				ImGui::End();
			}
			else
			{
				if (!drawTileInformation)
					return;

				auto gridPosRounded = world.toStrategaRounded(mousePos);
				if (!currentGameState->isInBounds(gridPosRounded))
					return;

				//Highlight tile drwable
				if(world.getTile(gridPosRounded))
					world.getTile(gridPosRounded)->isHighlighted = true;

				ImGui::SetNextWindowSize(ImVec2(300, 170), ImGuiCond_Always);
				ImGui::SetNextWindowPos(ImVec2(0, static_cast<float>(window.getSize().y)), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
				ImGui::Begin("Tile Information"/*, NULL, window_flags*/);

				auto& tileType = currentGameState->getGameInfo()->getTileType(currentGameState->getTileAtConst(gridPosRounded).getTileTypeID());

				ImGui::Text("%s", tileType.getName().c_str());
				ImGui::Columns(2, "mixed");
				ImGui::SetColumnWidth(0, 100.0f);

				ImGui::Separator();

				//Add units

				auto texture = renderTarget.getResourceManager().getTileSprite(tileType).createSprite();

				ImGui::Image(
					texture,
					sf::Vector2f(100, 100),
					sf::Color::White,
					sf::Color::Transparent);


				ImGui::NextColumn();
				ImGui::Text("Default parameters: ");

				if(tileType.isWalkable())
					ImGui::BulletText("Is walkable: true");
				else
					ImGui::BulletText("Is walkable: false");

				if(tileType.blockSight())
					ImGui::BulletText("Block sight: true");
				else
					ImGui::BulletText("Block sight: false");

				if(tileType.isDefaultTile())
					ImGui::BulletText("Is default tile: true");
				else
					ImGui::BulletText("Is default tile: false");

				if (tileType.getParameters().size() > 0)
				{
					ImGui::Text("Parameters: ");

					for (const auto& parameter : tileType.getParameters())
					{
						//Double to string with 2 precision
						std::stringstream stream;
						stream << std::fixed << std::setprecision(2) << currentGameState->getTileAtConst(gridPosRounded).getParameter(parameter.second.getName());
						std::string valueParameter = stream.str();

						std::string parameterInfo = parameter.second.getName() + ": " + valueParameter;
						ImGui::BulletText("%s", parameterInfo.c_str());
					}
				}			

				ImGui::End();
			}

		}
	}
}