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

    enum AnimationType
    {
        None = 1 << 0,
        Move = 1 << 1,
        Dissappear = 1 << 2,
        Appear = 1 << 3
    };

    inline AnimationType operator~ (AnimationType a) { return static_cast<AnimationType>(~static_cast<std::underlying_type<AnimationType>::type>(a)); }
    inline AnimationType operator| (AnimationType a, AnimationType b) { return static_cast<AnimationType>(static_cast<std::underlying_type<AnimationType>::type>(a) | static_cast<std::underlying_type<AnimationType>::type>(b)); }
    inline AnimationType operator& (AnimationType a, AnimationType b) { return static_cast<AnimationType>(static_cast<std::underlying_type<AnimationType>::type>(a) & static_cast<std::underlying_type<AnimationType>::type>(b)); }
    inline AnimationType operator^ (AnimationType a, AnimationType b) { return static_cast<AnimationType>(static_cast<std::underlying_type<AnimationType>::type>(a) ^ static_cast<std::underlying_type<AnimationType>::type>(b)); }
    inline AnimationType& operator|= (AnimationType& a, AnimationType b) { return reinterpret_cast<AnimationType&>(reinterpret_cast<std::underlying_type<AnimationType>::type&>(a) |= static_cast<std::underlying_type<AnimationType>::type>(b)); }
    inline AnimationType& operator&= (AnimationType& a, AnimationType b) { return reinterpret_cast<AnimationType&>(reinterpret_cast<std::underlying_type<AnimationType>::type&>(a) &= static_cast<std::underlying_type<AnimationType>::type>(b)); }
    inline AnimationType& operator^= (AnimationType& a, AnimationType b) { return reinterpret_cast<AnimationType&>(reinterpret_cast<std::underlying_type<AnimationType>::type&>(a) ^= static_cast<std::underlying_type<AnimationType>::type>(b)); }

    //AnimationType operator|(AnimationType lhs, AnimationType rhs) {
    //    return static_cast<AnimationType>(
    //        static_cast<std::underlying_type_t<AnimationType>>(lhs) |
    //        static_cast<std::underlying_type_t<AnimationType>>(rhs)
    //        );
    //}

    //AnimationType operator&(AnimationType lhs, AnimationType rhs) {
    //    return static_cast<AnimationType>(
    //        static_cast<std::underlying_type_t<AnimationType>>(lhs) &
    //        static_cast<std::underlying_type_t<AnimationType>>(rhs)
    //        );
    //}

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
            animation |= AnimationType::Move;
        }
        
        void dissappear()
        {
            alpha = 1;
            isAnimating = true;
            animation |= AnimationType::Dissappear;
        }

        void appear()
        {
            alpha = 0;
            isAnimating = true;
            animation |= AnimationType::Appear;
        }
    };

}