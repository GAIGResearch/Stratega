#pragma once
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/NewGUI/SGADrawable.h>
#include <Stratega/NewGUI/SGARenderTarget.h>
#include <SFML/System/Vector2.hpp>

namespace SGA
{
    static const int  TILE_ORIGIN_X = 0;
    static const int TILE_ORIGIN_Y = 112;

    struct World
    {
        sf::Vector2f xBaseVector;
        sf::Vector2f yBaseVector;
        Vector2i size;

        std::vector<SGADrawable> drawableList;
        World() = default;
        World(const sf::Vector2f& xBaseVector, const sf::Vector2f& yBaseVector, const  Vector2i size);

        sf::Vector2f toSFML(const Vector2f& pos) const;

        sf::Vector2f toSFML(const Vector2i& pos) const;

        Vector2f toStratega(const sf::Vector2f& pos) const;
        Vector2i toStrategaRounded(const sf::Vector2f& pos) const;

        static World createIsometricGrid(int tileWidth, int tileHeight, const Vector2i size);

        static World createRectangleGrid(int tileWidth, int tileHeight, const Vector2i size);

        void update(const GameState& state);

        void render(SGARenderTarget& renderTarget);
    };
}