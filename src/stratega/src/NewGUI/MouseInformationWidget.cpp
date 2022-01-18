#include <Stratega/NewGUI/MouseInformationWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>     // std::cout, std::fixed
#include <iomanip> 
#include <sstream>
namespace SGA
{
	MouseInformationWidget::MouseInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* fm):
		SGAWidget(widgetName, newWindow, newWorld, fm)
	{
	}

	void MouseInformationWidget::update(const GameState& state)
	{
		
	}
	
	void MouseInformationWidget::render(SGARenderTarget& renderTarget)
	{
		auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		auto gridPos = world.toStratega(mousePos);

		if (drawGridTile)
		{
			//Draw mouse tile
			sf::VertexArray selectedTile(sf::Lines, 8);
			
			auto gridPosI = SGA::Vector2i(std::floor(gridPos.x), std::floor(gridPos.y));
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

		ImGui::Begin("Mouse info");
		std::string gridPosText = "Grid Position: (" + std::to_string(gridPos.x) + "," + std::to_string(gridPos.y) + ")";
		auto strategaMousPos = world.toStrategaRounded(mousePos);
		std::string gridMousePosText = "Grid Position Round: (" + std::to_string(strategaMousPos.x) + "," + std::to_string(strategaMousPos.y) + ")";
		std::string windowMousePosText = "Mouse Position Round: (" + std::to_string((int)mousePos.x) + "," + std::to_string((int)mousePos.y) + ")";
		ImGui::Text(gridMousePosText.c_str());
		ImGui::Text(windowMousePosText.c_str());
		ImGui::Checkbox("Draw mouse over grid", &drawGridTile);
		ImGui::End();
	}

	GridInformationWidget::GridInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* fm) :
		SGAWidget(widgetName, newWindow, newWorld, fm),
		currentGameState(nullptr)
	{
	}

	void GridInformationWidget::update(const GameState& state)
	{
		currentGameState = &state;
	}

	void GridInformationWidget::render(SGARenderTarget& renderTarget)
	{
		ImGui::Begin("Grid information");
		ImGui::Checkbox("Draw entity information", &drawEntityInformation);
		ImGui::Checkbox("Draw tile information", &drawTileInformation);
		ImGui::End();
		
		{

			auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			auto gridPos = world.toStratega(mousePos);
			const auto* entity = currentGameState->getEntityAt(gridPos);

			if (entity)
			{
				if (!drawEntityInformation)
					return;
				//ImGuiWindowFlags window_flags = 0;
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
				world.getEntity(entity->getID())->isHighlighted=true;

				ImGui::SetNextWindowSize(ImVec2(300, 270), ImGuiCond_Always);
				ImGui::SetNextWindowPos(ImVec2(0, static_cast<float>(window.getSize().y)), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
				ImGui::Begin("Entity Information"/*, NULL, window_flags*/);

				auto& entityType = entity->getEntityType();

				ImGui::Text(entityType.getName().c_str());
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
				ImGui::Text(owner.c_str());
				ImGui::Text("Parameters: ");

				int parameterIndex = 0;
				for (const auto& parameter : entityType.getParameters())
				{
					//Double to string with 2 precision
					std::stringstream stream;
					stream << std::fixed << std::setprecision(2) << entity->getParameter(parameter.second.getName());
					std::string valueParameter = stream.str();

					std::string parameterInfo = parameter.second.getName() + ": " + valueParameter;
					ImGui::BulletText(parameterInfo.c_str());
				}

				ImGui::NextColumn();

				ImGui::Columns(1);
				ImGui::Separator();

				ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;
				ImGui::BeginChild("help", ImVec2(0, 80), true, child_flags);


				for (auto entity : currentGameState->getPlayerEntities(entity->getOwnerID()/*fowSettings.selectedPlayerID*/))
				{
					//Check if entity have sprite
					auto searchedEntityType = entity.getEntityType();
					//Add units

					if (ImGui::ImageButton(renderTarget.getResourceManager().getEntitySprite(searchedEntityType).createSprite(), sf::Vector2f(50, 50), -10))
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

				auto& tileType = currentGameState->getGameInfo()->getTileType(currentGameState->getTileAt(gridPosRounded).getTileTypeID());

				ImGui::Text(tileType.getName().c_str());
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
				ImGui::Text("Parameters: ");

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

				ImGui::End();
			}

		}
	}
}