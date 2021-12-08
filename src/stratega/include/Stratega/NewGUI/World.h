#pragma once
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/NewGUI/SGADrawable.h>
#include <SFML/System/Vector2.hpp>

namespace SGA
{
    struct World
    {
        sf::Vector2f xBaseVector;
        sf::Vector2f yBaseVector;

        std::vector<SGADrawable> drawableList;

        World(const sf::Vector2f& xBaseVector, const sf::Vector2f& yBaseVector);

        sf::Vector2f toSFML(const Vector2f& pos) const;

        sf::Vector2f toSFML(const Vector2i& pos) const;

        Vector2f toStratega(const sf::Vector2f& pos) const;
        Vector2i toStrategaRounded(const sf::Vector2f& pos) const;

        static World createIsometricGrid(int tileWidth, int tileHeight);

        static World createRectangleGrid(int tileWidth, int tileHeight);

        void update(const GameState& state);

        void render();
    };
}