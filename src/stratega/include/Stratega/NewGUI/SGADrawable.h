#pragma once
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/GameState.h>

namespace SGA {
    class SGARenderTarget;

    
    struct SGADrawable
    {
        Vector2f position;
        const int zPosition;
        bool isAnimating;
        float alpha;
        bool shouldRemove = false;
        bool isHighlighted = false;

        SGADrawable(const Vector2f& newPosition, int newZPosition)
            : position(newPosition), zPosition(newZPosition), isAnimating(false), alpha (1)
        {

        }

        virtual void update(float dt)=0;

        virtual void render(SGARenderTarget& renderTarget) const = 0;

        void updatePosition(const Vector2f& newPosition)
        {
            position = newPosition;
        }

        Vector2f Interpolate(  const Vector2f& pointA, const Vector2f& pointB, float factor)
        {
            if (factor > 1.f)
                factor = 1.f;

            else if (factor < 0.f)
                factor = 0.f;

            return pointA + (pointB - pointA) * factor;
        }

        float Interpolate(  const float& pointA, const float& pointB, float factor)
        {
            if (factor > 1.f)
                factor = 1.f;

            else if (factor < 0.f)
                factor = 0.f;

            return pointA + (pointB - pointA) * factor;
        }
    };

    struct SGADrawableTile: public SGADrawable
    {
        const TileType& type;

        SGADrawableTile(const Vector2f& newPosition, int newZPosition, const TileType& newType);

        void update(float dt) override;

        void render(SGARenderTarget& renderTarget) const override;
    };

    enum class AnimationType
    {
        None,
        Move,
        Dissappear,
        Appear
    };

    struct SGADrawableEntity: public SGADrawable
    {
        const EntityType& type;
        const int entityID;
        const int playerID;
        Vector2f targetPosition;
        AnimationType animation;
        SGADrawableEntity(const Vector2f& newPosition, int newZPosition, const EntityType& newType, const int newEntityID, const int newPlayerID);

        void update(float dt) override;

        void render(SGARenderTarget& renderTarget) const override;

        void moveTo(const Vector2f& newPosition)
        {
            isAnimating = true;
            targetPosition = newPosition;
            animation = AnimationType::Move;
        }
        
        void dissappear()
        {
            alpha = 1;
            isAnimating = true;
            animation = AnimationType::Dissappear;
        }

        void appear()
        {
            alpha = 0;
            isAnimating = true;
            animation = AnimationType::Appear;
        }
    };

}