#include <Stratega/NewGUI/WorldControllerWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui.h>
namespace SGA
{
	WorldControllerWidget::WorldControllerWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld):
		SGAWidget(widgetName, newWindow, newWorld)
	{
	}

	void WorldControllerWidget::update(const GameState& state)
	{

	}
	
	void WorldControllerWidget::render(SGARenderTarget& renderTarget)
	{
		ImGui::Begin("World Controller");
		std::string text;
		text = "XRatio.x";
		ImGui::DragFloat(text.c_str(), &world.xBaseVector.x, 1, -200, 200);
		text = "XRatio.y";
		ImGui::DragFloat(text.c_str(), &world.xBaseVector.y, 1, -200, 200);
		text = "YRatio.x";
		ImGui::DragFloat(text.c_str(), &world.yBaseVector.x, 1, -200, 200);
		text = "YRatio.y";
		ImGui::DragFloat(text.c_str(), &world.yBaseVector.y, 1, -200, 200);

		ImGui::Checkbox("Interpolate GameStates", &world.interpolateStates);
		ImGui::Checkbox("Interpolation animations enabled", &world.enableInterpolationAnimations);
		ImGui::End();
	}
}