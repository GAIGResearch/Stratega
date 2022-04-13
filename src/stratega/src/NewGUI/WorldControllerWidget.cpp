#include <Stratega/NewGUI/WorldControllerWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui.h>
namespace SGA
{
	WorldControllerWidget::WorldControllerWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFM):
		SGAWidget(widgetName, newWindow, newWorld, newFM)
	{
	}

	void WorldControllerWidget::update(const GameState& /*state*/)
	{

	}
	
	void WorldControllerWidget::render(SGARenderTarget& /*renderTarget*/)
	{
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 30), ImGuiCond_FirstUseEver);
		ImGui::Begin("World Controller");

		ImGui::Checkbox("Draw Tiles", &world.drawTiles);
		ImGui::Checkbox("Draw Entities", &world.drawEntities);

		ImGui::Separator();
		std::string temp = "Drawable number: " + std::to_string(world.drawableList.size());
		ImGui::Text("%s", temp.c_str());
		temp = "Animators number playing: " + std::to_string(world.animatingNumber);

		if (world.animationsRunning())
			ImGui::Text("Animations running: true");
		else
			ImGui::Text("Animations running: false");

		ImGui::Text("%s", temp.c_str());

		ImGui::Separator();
		ImGui::Checkbox("Interpolate GameStates", &world.interpolateStates);

		if(world.interpolateStates)
			ImGui::Checkbox("Interpolation animations enabled", &world.enableInterpolationAnimations);

		ImGui::Separator();
		std::string text;
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