#pragma once
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/GameState.h>

namespace SGA {
    class SGARenderTarget;

    
    struct SGADrawable
    {
        const Vector2f position;
        const int zPosition;

        SGADrawable(const Vector2f& newPosition, int newZPosition)
            : position(newPosition), zPosition(newZPosition)
        {

        }

        virtual void update()=0;

        virtual void render(SGARenderTarget& renderTarget) const = 0;
    };

    struct SGADrawableTile: public SGADrawable
    {
        const TileType& type;

        SGADrawableTile(const Vector2f& newPosition, int newZPosition, const TileType& newType);

        void update() override;

        void render(SGARenderTarget& renderTarget) const override;
    };

    struct SGADrawableEntity: public SGADrawable
    {
        const EntityType& type;

        SGADrawableEntity(const Vector2f& newPosition, int newZPosition, const EntityType& newType);

        void update() override;

        void render(SGARenderTarget& renderTarget) const override;
    };

}