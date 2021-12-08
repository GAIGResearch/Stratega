#pragma once
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/GameState.h>

namespace SGA {

    struct SGADrawable
    {
        Vector2f position;
        int zPosition;

        SGADrawable(const Vector2f& newPosition, int newZPosition)
            : position(newPosition), zPosition(newZPosition)
        {
        }

        void update()
        {

        }

        void render()
        {

        }        
    };
}