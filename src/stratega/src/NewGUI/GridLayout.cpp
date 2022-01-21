#include <Stratega/NewGUI/GridLayout.h>

namespace SGA {
   GridLayout::GridLayout(const sf::Vector2f& newXBaseVector, const sf::Vector2f& newYBaseVector)
      : xBaseVector(newXBaseVector), yBaseVector(newYBaseVector)
   {
   }

   sf::Vector2f GridLayout::toSFML(const Vector2f& pos) const
   {
      auto x = static_cast<float>(pos.x) * xBaseVector.x + static_cast<float>(pos.y) * xBaseVector.y;
      auto y = static_cast<float>(pos.x) * yBaseVector.x + static_cast<float>(pos.y) * yBaseVector.y;
      return {x, y};
   }

   sf::Vector2f GridLayout::toSFML(const Vector2i& pos) const
   {
      return toSFML(Vector2f(pos.x, pos.y));  
   }

   Vector2f GridLayout::toStratega(const sf::Vector2f& pos) const
   {
      // Cross product for the matrix used below
      auto cross = xBaseVector.x * yBaseVector.y - xBaseVector.y * yBaseVector.x;

      // No idea what exactly happens, but this is a matrix multiplication
      auto y =  xBaseVector.x * pos.y - yBaseVector.x * pos.x;
      y /= cross;
      auto x = -xBaseVector.y * pos.y + yBaseVector.y * pos.x;
      x /= cross;

      return {static_cast<double>(x), static_cast<double>(y)};
   }

   Vector2i GridLayout::toStrategaRounded(const sf::Vector2f& pos) const
   {
      auto convertedPos = toStratega(pos);
      return {
         static_cast<int>(std::floor(convertedPos.x)),
         static_cast<int>(std::floor(convertedPos.y))
      };
   }

   GridLayout GridLayout::createIsometricGrid(int tileWidth, int tileHeight)
   {
      return {sf::Vector2f(static_cast<float>(tileWidth) / 2.0f, static_cast<float>(-tileWidth) / 2.0f), sf::Vector2f(static_cast<float>(tileHeight) / 2.0f, static_cast<float>(tileHeight) / 2.0f)};
   }

   GridLayout GridLayout::createRectangleGrid(int tileWidth, int tileHeight)
   {
      return {sf::Vector2f(static_cast<float>(tileWidth), 0), sf::Vector2f(0,static_cast<float>(tileHeight))};
   }

}