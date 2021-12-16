#include <Stratega/NewGUI/MouseInformationWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui-SFML.h>
#include <imgui.h>
namespace SGA
{
	MouseInformationWidget::MouseInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld):
		SGAWidget(widgetName, newWindow, newWorld)
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
}