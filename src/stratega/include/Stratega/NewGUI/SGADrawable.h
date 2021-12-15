#pragma once
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/GameState.h>

namespace SGA {
    class SGARenderTarget;

    struct SGADrawable
    {
        const TileType& type;
        Vector2i position;
        int zPosition;

        SGADrawable(const Vector2i& newPosition, int newZPosition, const TileType& newType);

        void update();

        void render(SGARenderTarget& renderTarget);
    };
}