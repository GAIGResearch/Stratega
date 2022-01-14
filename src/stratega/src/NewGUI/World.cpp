#include<Stratega/NewGUI/World.h>
#include <algorithm>

namespace SGA
{
    World::World(const sf::Vector2f& xBaseVector, const sf::Vector2f& yBaseVector, const Vector2i size)
        : xBaseVector(xBaseVector), yBaseVector(yBaseVector), size(size)
    {
    }

    sf::Vector2f World::toSFML(const Vector2f& pos) const
    {
        auto x = static_cast<float>(pos.x) * xBaseVector.x + static_cast<float>(pos.y) * xBaseVector.y;
        auto y = static_cast<float>(pos.x) * yBaseVector.x + static_cast<float>(pos.y) * yBaseVector.y;
        return { x, y };
    }

    sf::Vector2f World::toSFML(const Vector2i& pos) const
    {
        return toSFML(Vector2f(pos.x, pos.y));
    }

    Vector2f World::toStratega(const sf::Vector2f& pos) const
    {
        // Cross product for the matrix used below
        auto cross = xBaseVector.x * yBaseVector.y - xBaseVector.y * yBaseVector.x;

        // No idea what exactly happens, but this is a matrix multiplication
        auto y = xBaseVector.x * pos.y - yBaseVector.x * pos.x;
        y /= cross;
        auto x = -xBaseVector.y * pos.y + yBaseVector.y * pos.x;
        x /= cross;

        return { static_cast<double>(x), static_cast<double>(y) };
    }

    Vector2i World::toStrategaRounded(const sf::Vector2f& pos) const
    {
        auto convertedPos = toStratega(pos);
        return {
           static_cast<int>(std::floor(convertedPos.x)),
           static_cast<int>(std::floor(convertedPos.y))
        };
    }

    World World::createIsometricGrid(int tileWidth, int tileHeight, const Vector2i size)
    {
        return { sf::Vector2f(tileWidth / 2., -tileWidth / 2.), sf::Vector2f(tileHeight / 2., tileHeight / 2.) , size};
    }

    World World::createRectangleGrid(int tileWidth, int tileHeight, const Vector2i size)
    {
        return { sf::Vector2f(tileWidth, 0), sf::Vector2f(0, tileHeight), size};
    }

    void World::sortDrawables()
    {
        //Oder by y
        std::sort(drawableList.begin(), drawableList.end(), [](const std::unique_ptr<SGADrawable>& lhs, const std::unique_ptr<SGADrawable>& rhs)
        {
            if (lhs->zPosition != lhs->zPosition)
                return (lhs->zPosition < lhs->zPosition);

            if (lhs->position.y < lhs->position.y)
                return(lhs->position.x < lhs->position.x);
            else
                return(lhs->position.x > lhs->position.x);
        });
    } 
    
    void World::init(const GameState& state, const RenderConfig& renderConfig)
    {
        resetDrawables();
        addTileDrawables(state);       
    }
    
    void World::update(const GameState& state)
    {
        lastUpdatedState = state;

        if (interpolateStates)
        {
            if (!interpolateStatesBefore)
            {
                resetDrawables();
                addTileDrawables(state);
                
            }
            interpolateStatesBefore = interpolateStates;

            interpolateEntityDrawables(state);
        }       
        else
        {
            resetDrawables();
            addTileDrawables(state);
            addEntityDrawables(state);
        }

        sortDrawables();
    }

    void World::render(SGARenderTarget& renderTarget, float dt)
    {
        animatingNumber = 0;

        for (auto& drawable : drawableList)
        {
            auto* drawableEntity = dynamic_cast<SGADrawableEntity*>(drawable.get());
            if ((drawableEntity&&drawEntities) || (!drawableEntity && drawTiles))
            {
                drawable->update(dt);                
                drawable->render(renderTarget);
            }            

            //Check if is animating
            if (drawable->isAnimating)
                animatingNumber++;

            drawable->isHighlighted=false;
        }
    }
}