#pragma once
#include <Stratega/Representation/Vector2.h>
#include <SFML/System/Vector2.hpp>

namespace SGA {
   struct GridLayout {
      sf::Vector2f xBaseVector;
      sf::Vector2f yBaseVector;

      GridLayout(const sf::Vector2f& newXBaseVector, const sf::Vector2f& newYBaseVector);

      sf::Vector2f toSFML(const Vector2f& pos) const;
      sf::Vector2f toSFML(const Vector2i& pos) const;
      
      Vector2f toStratega(const sf::Vector2f& pos) const;
      Vector2i toStrategaRounded(const sf::Vector2f& pos) const;

      static GridLayout createIsometricGrid(int tileWidth, int tileHeight);
      static GridLayout createRectangleGrid(int tileWidth, int tileHeight);
   };
}