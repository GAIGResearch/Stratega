#include <Stratega/NewGUI/GridLayoutWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui.h>
namespace SGA
{
	GridLayoutWidget::GridLayoutWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFm):
		SGAWidget(widgetName, newWindow, newWorld, newFm)
	{
	}

	void GridLayoutWidget::update(const GameState& /*state*/)
	{

	}
	
	void GridLayoutWidget::render(SGARenderTarget& renderTarget)
	{
		// We need 2 entries for each line and we need 2 extra lines (aka 4 entries) because a
		// rectangle always needs 2 lines more than it's width + height
		Vector2i xAxis;
		Vector2i yAxis;
		if (drawCompleteGrid)
		{
			xAxis = { -500,500 };
			yAxis = { -500,500 };
		}
		else
		{
			xAxis = { 0,world.size.x };
			yAxis = { 0,world.size.y };
		}
		sf::VertexArray vertices(sf::Lines, 4 + 2 * (xAxis.y * yAxis.y));
		for (int y = yAxis.x; y < yAxis.y; y++)
		{
			vertices[(y+(xAxis.x*-1)) * 2] = sf::Vertex(world.toSFML(Vector2i(xAxis.x, y)), sf::Color::White);
			vertices[(y+ (xAxis.x * -1)) * 2 + 1] = sf::Vertex(world.toSFML(Vector2i(xAxis.y, y)), sf::Color::White);
		}

		for (int x = xAxis.x; x < xAxis.y; x++)
		{
			auto index = (yAxis.y + 1) * 2 + (x + (xAxis.x * -1)) * 2;
			vertices[index] = sf::Vertex(world.toSFML(Vector2i(x, yAxis.x)), sf::Color::White);
			vertices[index + 1] = sf::Vertex(world.toSFML(Vector2i(x, yAxis.y)), sf::Color::White);
		}

		renderTarget.drawVertices(vertices);

		ImGui::SetNextWindowPos(ImVec2(20, 170), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
		ImGui::Begin("Grid layout");
		ImGui::Checkbox("Draw outbounds", &drawCompleteGrid);
		ImGui::End();
	}
}