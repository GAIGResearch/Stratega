#pragma once
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include <Stratega/Representation/Vector2.h>
#include <Stratega/NewGUI/GridLayout.h>

namespace SGA {
class StrategaRenderer {
  public:
   StrategaRenderer()
       : xRatio(1, 0),
         yRatio(0, 1),
         tileWidth(200),
         tileHeight(100),
         window(sf::VideoMode(800, 400), "Stratega GUI", sf::Style::Default | sf::Style::Titlebar),
         layout(GridLayout::createIsometricGrid(tileWidth, tileHeight))
   {
      sf::View view = window.getView();
      view.setCenter(0, (float) (window.getSize().y / 2.));
      view.setSize(window.getDefaultView().getSize());  // Reset the size
      view.zoom(2);  // Apply the zoom level (this transforms the view)
      window.setView(view);
      window.setFramerateLimit(60);

      ImGui::SFML::Init(window);
   }

   void render()
   {
      // check all the window's events that were triggered since the last iteration of the loop
      sf::Event event;
      while(window.pollEvent(event)) {
         ImGui::SFML::ProcessEvent(event);
      }

      window.clear(sf::Color::Black);

      // We need 2 entries for each line and we need 2 extra lines (aka 4 entries) because a
      // rectangle always needs 2 lines more than it's width + height
      sf::VertexArray vertices(sf::Lines, 4 + 2 * (10 + 10));
      for(int y = -5; y < 6; y++) {
         vertices[(y+5) * 2] = sf::Vertex(layout.toSFML(Vector2i(-5, y)), sf::Color::White);
         vertices[(y+5) * 2 + 1] = sf::Vertex(layout.toSFML(Vector2i(5, y)), sf::Color::White);
      }

      for(int x = -5; x < 6; x++) {
         auto index = (10 + 1) * 2 + (x+5) * 2;
         vertices[index] = sf::Vertex(layout.toSFML(Vector2i(x, -5)), sf::Color::White);
         vertices[index + 1] = sf::Vertex(layout.toSFML(Vector2i(x, 5)), sf::Color::White);
      }
      window.draw(vertices);

      auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
      sf::VertexArray selectedTile(sf::Lines, 8);
      auto gridPos = layout.toStratega(mousePos);
      auto gridPosI = SGA::Vector2i(std::floor(gridPos.x), std::floor(gridPos.y));
      selectedTile[0] = sf::Vertex(layout.toSFML(gridPosI), sf::Color::Yellow);
      selectedTile[1] = sf::Vertex(layout.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y)), sf::Color::Yellow);

      selectedTile[2] = sf::Vertex(layout.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y)), sf::Color::Yellow);
      selectedTile[3] = sf::Vertex(layout.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y + 1)), sf::Color::Yellow);

      selectedTile[4] = sf::Vertex(layout.toSFML(Vector2i(gridPosI.x + 1, gridPosI.y + 1)), sf::Color::Yellow);
      selectedTile[5] = sf::Vertex(layout.toSFML(Vector2i(gridPosI.x, gridPosI.y + 1)), sf::Color::Yellow);

      selectedTile[6] = sf::Vertex(layout.toSFML(Vector2i(gridPosI.x, gridPosI.y + 1)), sf::Color::Yellow);
      selectedTile[7] = sf::Vertex(layout.toSFML(Vector2i(gridPosI.x, gridPosI.y)), sf::Color::Yellow);

      window.draw(selectedTile);

      sf::CircleShape s(4);
      s.setOrigin({2, 2});
      s.setPosition(layout.toSFML(SGA::Vector2f{0, 0}));
      s.setFillColor(sf::Color::Red);
      window.draw(s);

      ImGui::SFML::Update(window, deltaClock.restart());

      ImGui::Begin("Test window");

      

      std::string gridPosText = "Grid Position: (" + std::to_string(gridPos.x) + "," + std::to_string(gridPos.y) + ")";
      ImGui::Text(gridPosText.c_str());
      std::string text = "Width";
      text = "XRatio.x";
      ImGui::DragFloat(text.c_str(), &layout.xBaseVector.x, 1, -200, 200);
      text = "XRatio.y";
      ImGui::DragFloat(text.c_str(), &layout.xBaseVector.y, 1, -200, 200);
      text = "YRatio.x";
      ImGui::DragFloat(text.c_str(), &layout.yBaseVector.x, 1, -200, 200);
      text = "YRatio.y";
      ImGui::DragFloat(text.c_str(), &layout.yBaseVector.y, 1, -200, 200);
      ImGui::End();

      ImGui::SFML::Render(window);

      window.display();
   }
   
   sf::Clock deltaClock;
   sf::Vector2f xRatio;
   sf::Vector2f yRatio;
   int tileWidth;
   int tileHeight;
   sf::RenderWindow window;
   GridLayout layout;
};
}  // namespace SGA