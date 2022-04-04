#pragma once
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/NewGUI/SGADrawable.h>
#include <Stratega/NewGUI/SGARenderTarget.h>
#include <Stratega/NewGUI/FogOfWarSettings.h>
#include <SFML/System/Vector2.hpp>
#include <Stratega/Configuration/RenderConfig.h>
#include <memory>
namespace SGA
{
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
        World(const sf::Vector2f& newXBaseVector, const sf::Vector2f& newYBaseVector, const  Vector2i newSize, std::unordered_set<int>& newSelectedEntities, const FogOfWarSettings& newSettings);

        sf::Vector2f toSFML(const Vector2f& pos) const;

        sf::Vector2f toSFML(const Vector2i& pos) const;

        Vector2f toStratega(const sf::Vector2f& pos) const;
        Vector2i toStrategaRounded(const sf::Vector2f& pos) const;

        static World createIsometricGrid(int tileWidth, int tileHeight, const Vector2i size, std::unordered_set<int>& newSelectedEntities, const FogOfWarSettings& newSettings);

        static World createRectangleGrid(int tileWidth, int tileHeight, const Vector2i size, std::unordered_set<int>& newSelectedEntities, const FogOfWarSettings& newSettings);

        void init(const GameState& state, const GameState& fowState, const RenderConfig& renderConfig);
        void update(const GameState& state, const GameState& fowState);
        void resetDrawables()
        {
            drawableList.clear();
        }
        void addTileDrawables(const GameState& state, const GameState& fowState)
        {
            //Update list of drawable list
            for (int x = 0; x < state.getBoardWidth(); ++x)
            {
                for (int y = 0; y < state.getBoardHeight(); ++y)
                {
                    // get the current tile
                    const auto& tile = state.getTileAtConst({ x, y });
                    const auto& tileFOW =fowState.getTileAtConst({ x, y });
                    const auto& tileType = state.getGameInfo()->getTileType(tile.getTileTypeID());
                    const auto& tileTypeFOW = fowState.getGameInfo()->getTileType(tileFOW.getTileTypeID());
                    Vector2f position(x, y);
                    if (tileTypeFOW.getID()==-1&& fowSettings->renderFogOfWar)
                    {
                        drawableList.emplace_back(std::make_unique<SGADrawableTile>(position, -1, tileTypeFOW));
                    }
                    else
                    {
                        drawableList.emplace_back(std::make_unique<SGADrawableTile>(position, -1, tileType));
                    }
                }
            }
        }
        void addEntityDrawables(const GameState& state, const GameState& fowState)
        {
            if(fowSettings->renderFogOfWar)
                for (auto& entity : fowState.getEntities())
                {                
                    const auto& position = entity.getPosition();
                    drawableList.emplace_back(std::make_unique<SGADrawableEntity>(position, entity.getEntityType().getGrid(), entity.getEntityType(), entity.getID(), entity.getOwnerID()));
                }
            else
                for (auto& entity : state.getEntities())
                {
                    const auto& position = entity.getPosition();
                    drawableList.emplace_back(std::make_unique<SGADrawableEntity>(position, entity.getEntityType().getGrid(), entity.getEntityType(), entity.getID(), entity.getOwnerID()));
                }
        }
        void interpolateEntityDrawables(const GameState& state, const GameState& fowState)
        {
            auto it = drawableList.begin();
            while (it != drawableList.end())
            {
                //Remove entities that should be removed
                if (it->get()->shouldRemove)
                    it = drawableList.erase(it);
                else
                {
                    auto* drawableEntity = dynamic_cast<SGADrawableEntity*>(it->get());
                    if (drawableEntity)
                    {
                        if (enableInterpolationAnimations)
                        {
                            if (fowSettings->renderFogOfWar)
                            {
                                auto* foundEntity = fowState.getOnlyEntitiesConst(drawableEntity->entityID);
                                if (!foundEntity)
                                {
                                    drawableEntity->dissappear();
                                }
                            }
                            else
                            {
                                auto* foundEntity = state.getOnlyEntitiesConst(drawableEntity->entityID);
                                if (!foundEntity)
                                {
                                    drawableEntity->dissappear();
                                }
                            }
                            ++it;                            
                        }
                        else
                        {
                            it = drawableList.erase(it);
                        }                       
                    }
                    else
                    {
                        ++it;
                    }                     
                }
            }

            if (fowSettings->renderFogOfWar)
            {
                for (auto& entity : fowState.getEntities())
                {
                    if (hasEntity(entity.getID()))
                    {
                        const auto& position = entity.getPosition();
                        auto* drawableEntity = getEntity(entity.getID());
                        if (enableInterpolationAnimations)
                        {
                            drawableEntity->moveTo(position);
                            if(drawableEntity->isDissapearing())
                                drawableEntity->appear();
                        }
                        else
                        {
                            drawableEntity->updatePosition(position);
                        }
                    }
                    else
                    {
                        const auto& position = entity.getPosition();
                        drawableList.emplace_back(std::make_unique<SGADrawableEntity>(position, entity.getEntityType().getGrid(), entity.getEntityType(), entity.getID(), entity.getOwnerID(), 0));

                        if (enableInterpolationAnimations)
                        {
                            auto* drawableEntity = dynamic_cast<SGADrawableEntity*>(drawableList.back().get());
                            if (drawableEntity != nullptr)
                                if (!drawableEntity->isAnimating)
                                    drawableEntity->appear();
                        }
                    }
                }
            }
            else
            {
                for (auto& entity : state.getEntities())
                {
                    if (hasEntity(entity.getID()))
                    {
                        const auto& position = entity.getPosition();
                        auto* drawableEntity = getEntity(entity.getID());
                        if (enableInterpolationAnimations)
                        {
                            drawableEntity->moveTo(position);
                            if (drawableEntity->isDissapearing())
                                drawableEntity->appear();
                        }
                        else
                        {
                            drawableEntity->updatePosition(position);
                        }
                    }
                    else
                    {
                        const auto& position = entity.getPosition();
                        drawableList.emplace_back(std::make_unique<SGADrawableEntity>(position, entity.getEntityType().getGrid(), entity.getEntityType(), entity.getID(), entity.getOwnerID()));

                        if (enableInterpolationAnimations)
                        {
                            auto* drawableEntity = dynamic_cast<SGADrawableEntity*>(drawableList.back().get());
                            if(drawableEntity!=nullptr)
                                if (!drawableEntity->isAnimating)
                                    drawableEntity->appear();
                        }
                    }
                }
            }            
        }

        void interpolateTilesDrawables(const GameState& state, const GameState& fowState)
        {
            auto it = drawableList.begin();
            while (it != drawableList.end())
            {
                auto* drawableTile = dynamic_cast<SGADrawableTile*>(it->get());
                if (drawableTile)
                {
                    it = drawableList.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            
            addTileDrawables(state, fowState);
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
        const FogOfWarSettings& getFOWSettings()
        {
            return *fowSettings;
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
        GameState lastUpdatedStateFOW;
        bool interpolateStatesBefore = true;
        std::unordered_set<int>* selectedEntities;
        const FogOfWarSettings* fowSettings;
    };
}