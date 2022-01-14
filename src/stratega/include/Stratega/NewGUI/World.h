#pragma once
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/NewGUI/SGADrawable.h>
#include <Stratega/NewGUI/SGARenderTarget.h>
#include <SFML/System/Vector2.hpp>
#include <Stratega/Configuration/RenderConfig.h>
#include <memory>
namespace SGA
{
    static const int TILE_WIDTH = 256;
    static const int TILE_HEIGHT = 144;

    static const int  TILE_ORIGIN_X = 0;
    static const int TILE_ORIGIN_Y = 112;
    //Units
    static const int  UNIT_OFFSET_ORIGIN_X = (TILE_WIDTH / 2);
    static const int UNIT_OFFSET_ORIGIN_Y = static_cast<int>(static_cast<float>(TILE_HEIGHT) / 1.2);

    //Other sprites
    static const int  TILE_OFFSET_ORIGIN_X = 0/*TILE_WIDTH / 2*/;
    static const int TILE_OFFSET_ORIGIN_Y = 112/2;

    struct World
    {
        sf::Vector2f xBaseVector;
        sf::Vector2f yBaseVector;
        Vector2i size;

        bool drawEntities=true;
        bool drawTiles=true;

        bool interpolateStates=true;
        bool enableInterpolationAnimations=true;

        std::vector<std::unique_ptr<SGADrawable>> drawableList;

        int animatingNumber=0;

        World() = default;
        World(const sf::Vector2f& xBaseVector, const sf::Vector2f& yBaseVector, const  Vector2i size);

        sf::Vector2f toSFML(const Vector2f& pos) const;

        sf::Vector2f toSFML(const Vector2i& pos) const;

        Vector2f toStratega(const sf::Vector2f& pos) const;
        Vector2i toStrategaRounded(const sf::Vector2f& pos) const;

        static World createIsometricGrid(int tileWidth, int tileHeight, const Vector2i size);

        static World createRectangleGrid(int tileWidth, int tileHeight, const Vector2i size);

        void init(const GameState& state, const RenderConfig& renderConfig);
        void update(const GameState& state);
        void resetDrawables()
        {
            drawableList.clear();
        }
        void addTileDrawables(const GameState& state)
        {
            //Update list of drawable list
            for (int x = 0; x < state.getBoardWidth(); ++x)
            {
                for (int y = 0; y < state.getBoardHeight(); ++y)
                {
                    // get the current tile
                    const auto& tile = state.getTileAt({ x, y });
                    const auto& tileType = state.getGameInfo()->getTileType(tile.getTileTypeID());
                    Vector2f position(x, y);
                    drawableList.emplace_back(std::make_unique<SGADrawableTile>(position, 0, tileType));
                }
            }
        }
        void addEntityDrawables(const GameState& state)
        {
            for (auto& entity : state.getEntities())
            {                
                const auto& position = entity.getPosition();
                drawableList.emplace_back(std::make_unique<SGADrawableEntity>(position, 0, entity.getEntityType(), entity.getID(), entity.getOwnerID()));
            }
        }
        void interpolateEntityDrawables(const GameState& state)
        {
            auto it = drawableList.begin();
            while (it != drawableList.end())
            {
                //use *it here
                if (it->get()->shouldRemove)
                    it = drawableList.erase(it);
                else
                {
                    if (enableInterpolationAnimations)
                    {
                        auto* drawableEntity = dynamic_cast<SGADrawableEntity*>(it->get());
                        if (drawableEntity)
                        {
                            auto* foundEntity = state.getEntityConst(drawableEntity->entityID);
                            if (!foundEntity)
                            {
                                drawableEntity->dissappear();
                            }
                        }
                    }
                    ++it;
                }
            }

            for (auto& entity : state.getEntities())
            {
                if (hasEntity(entity.getID()))
                {
                    const auto& position = entity.getPosition();
                    auto* drawableEntity = getEntity(entity.getID());
                    if (enableInterpolationAnimations)
                    {
                        drawableEntity->moveTo(position);
                    }
                    else
                    {
                        drawableEntity->updatePosition(position);
                    }
                }
                else
                {
                    const auto& position = entity.getPosition();
                    drawableList.emplace_back(std::make_unique<SGADrawableEntity>(position, 0, entity.getEntityType(), entity.getID(), entity.getOwnerID()));

                    if (enableInterpolationAnimations)
                    {
                        auto* drawableEntity = dynamic_cast<SGADrawableEntity*>(drawableList.back().get());
                        if(!drawableEntity->isAnimating)
                            drawableEntity->appear();
                    }
                }
            }
        }
        void sortDrawables();
        bool animationsRunning()
        {
            return animatingNumber > 0;
        }

        bool hasEntity(const int entityID) const
        {
            for (auto& drawable : drawableList)
            {
                auto* entity = dynamic_cast<SGADrawableEntity*>(drawable.get());
                if (entity)
                    if (entity->entityID == entityID)
                        return true;
            }
            return false;
        }

        SGADrawableEntity* getEntity(const int entityID) const
        {
            for (auto& drawable : drawableList)
            {
                auto* entity = dynamic_cast<SGADrawableEntity*>(drawable.get());
                if (entity)
                    if (entity->entityID == entityID)
                        return entity;
            }

            return nullptr;
        }

        SGADrawableTile* getTile(Vector2i& pos) const
        {
            for (auto& drawable : drawableList)
            {
                auto* tile = dynamic_cast<SGADrawableTile*>(drawable.get());
                if (tile)
                {
                    if (tile->position.x == pos.x&& tile->position.y == pos.y)
                        return tile;
                }
            }

            return nullptr;
        }

        void render(SGARenderTarget& renderTarget, float dt);

    private:
        GameState lastUpdatedState;
        bool interpolateStatesBefore = true;
    };
}