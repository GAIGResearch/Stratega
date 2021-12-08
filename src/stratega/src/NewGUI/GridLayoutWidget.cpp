#include <Stratega/NewGUI/GridLayoutWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui.h>
namespace SGA
{
	GridLayout::GridLayout(World& newWorld):
		world(newWorld)
	{

	}

	void GridLayout::update(const GameState& state)
	{

	}

	void GridLayout::render(SGARenderTarget& renderTarget)
	{
		// We need 2 entries for each line and we need 2 extra lines (aka 4 entries) because a
		// rectangle always needs 2 lines more than it's width + height
		sf::VertexArray vertices(sf::Lines, 4 + 2 * (10 + 10));
		for (int y = -5; y < 6; y++) {
			vertices[(y + 5) * 2] = sf::Vertex(world.toSFML(Vector2i(-5, y)), sf::Color::White);
			vertices[(y + 5) * 2 + 1] = sf::Vertex(world.toSFML(Vector2i(5, y)), sf::Color::White);
		}

		for (int x = -5; x < 6; x++) {
			auto index = (10 + 1) * 2 + (x + 5) * 2;
			vertices[index] = sf::Vertex(world.toSFML(Vector2i(x, -5)), sf::Color::White);
			vertices[index + 1] = sf::Vertex(world.toSFML(Vector2i(x, 5)), sf::Color::White);
		}

		renderTarget.drawVertices(vertices);

		ImGui::Begin("Test window");
		//std::string gridPosText = "Grid Position: (" + std::to_string(gridPos.x) + "," + std::to_string(gridPos.y) + ")";
		//ImGui::Text(gridPosText.c_str());
		std::string text = "Width";
		text = "XRatio.x";
		ImGui::DragFloat(text.c_str(), &world.xBaseVector.x, 1, -200, 200);
		text = "XRatio.y";
		ImGui::DragFloat(text.c_str(), &world.xBaseVector.y, 1, -200, 200);
		text = "YRatio.x";
		ImGui::DragFloat(text.c_str(), &world.yBaseVector.x, 1, -200, 200);
		text = "YRatio.y";
		ImGui::DragFloat(text.c_str(), &world.yBaseVector.y, 1, -200, 200);
		ImGui::End();
	}
}