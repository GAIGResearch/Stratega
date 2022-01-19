#include<Stratega/NewGUI/World.h>
#include <algorithm>

namespace SGA
{
    World::World(const sf::Vector2f& xBaseVector, const sf::Vector2f& yBaseVector, const Vector2i size, std::unordered_set<int>& newSelectedEntities, const FogOfWarSettings& newSettings)
        : xBaseVector(xBaseVector), yBaseVector(yBaseVector), size(size), selectedEntities(&newSelectedEntities), fowSettings(&newSettings)
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
        return 
        {
           static_cast<int>(std::floor(convertedPos.x)),
           static_cast<int>(std::floor(convertedPos.y))
        };
    }

    World World::createIsometricGrid(int tileWidth, int tileHeight, const Vector2i size, std::unordered_set<int>& newSelectedEntities, const FogOfWarSettings& newSettings)
    {
        return { sf::Vector2f(tileWidth / 2., -tileWidth / 2.), sf::Vector2f(tileHeight / 2., tileHeight / 2.) , size, newSelectedEntities, newSettings };
    }

    World World::createRectangleGrid(int tileWidth, int tileHeight, const Vector2i size, std::unordered_set<int>& newSelectedEntities, const FogOfWarSettings& newSettings)
    {
        return { sf::Vector2f(tileWidth, 0), sf::Vector2f(0, tileHeight), size, newSelectedEntities, newSettings };
    }

    void World::sortDrawables()
    {
        //Oder by y
        std::sort(drawableList.begin(), drawableList.end(), [](const std::unique_ptr<SGADrawable>& lhs, const std::unique_ptr<SGADrawable>& rhs)
        {
            if (lhs->zPosition != rhs->zPosition)
                return (lhs->zPosition < rhs->zPosition);

            return std::tie(lhs->position.y, lhs->position.x) < std::tie(rhs->position.y, rhs->position.x);      

        });
    } 
    
    void World::init(const GameState& state, const GameState& fowState, const RenderConfig& renderConfig)
    {
        lastUpdatedState = state;
        lastUpdatedStateFOW = fowState;

        resetDrawables();
        addTileDrawables(lastUpdatedState, lastUpdatedStateFOW);
    }
    
    void World::update(const GameState& state, const GameState& fowState)
    {
        lastUpdatedState = state;
        lastUpdatedStateFOW = fowState;

        if (interpolateStates)
        {
            if (!interpolateStatesBefore)
            {
                resetDrawables();
                //addTileDrawables(state);                
            }
            interpolateStatesBefore = interpolateStates;

            interpolateTilesDrawables(lastUpdatedState, lastUpdatedStateFOW);
            interpolateEntityDrawables(lastUpdatedState, lastUpdatedStateFOW);
        }       
        else
        {
            resetDrawables();

            addEntityDrawables(lastUpdatedState, lastUpdatedStateFOW);
            addTileDrawables(lastUpdatedState, lastUpdatedStateFOW);
        }

        sortDrawables();
    }

    void World::render(SGARenderTarget& renderTarget, float dt)
    {
        //std::cout << "DT: " << dt << std::endl;
        animatingNumber = 0;

        for (auto& drawable : drawableList)
        {
            auto* drawableEntity = dynamic_cast<SGADrawableEntity*>(drawable.get());
            if ((drawableEntity&&drawEntities) || (!drawableEntity && drawTiles))
            {
                drawable->update(dt);                
                drawable->render(renderTarget);

                if(drawableEntity)
                {
                    if (selectedEntities->count(drawableEntity->entityID) != 0)
                        drawableEntity->isHighlighted = true;
                    else
                        drawable->isHighlighted = false;
                }                    
                else
                {
                    drawable->isHighlighted = false;
                }
                    
            }
            else
            {
                drawable->isHighlighted = false;
            }

            //Check if is animating
            if (drawable->isAnimating)
                animatingNumber++;

        }
    }
}