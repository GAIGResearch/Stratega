#include <Stratega/NewGUI/SGADrawable.h>

#include <Stratega/NewGUI/SGARenderTarget.h>
namespace SGA
{

    SGADrawableTile::SGADrawableTile(const Vector2f& newPosition, int newZPosition, const TileType& newType)
        : SGADrawable(newPosition, newZPosition), type(newType)
    {
    }

    void SGADrawableTile::update(float dt)
    {

    }

    void SGADrawableTile::render(SGARenderTarget& renderTarget) const
    {
        renderTarget.drawTile(position, type, 255*alpha);
    }

    SGADrawableEntity::SGADrawableEntity(const Vector2f& newPosition, int newZPosition, const EntityType& newType, const int newEntityID)
        : SGADrawable(newPosition, newZPosition), type(newType), entityID(newEntityID), animation(AnimationType::None)
    {
    }

    void SGADrawableEntity::update(float dt)
    {
        if (isAnimating)
        {
            switch (animation)
            {
            case AnimationType::Move:
                if (targetPosition != position && targetPosition.distance(position)>0.1f)
                {
                    position = Interpolate(position, targetPosition, dt * 0.001f);
                }
                else
                {
                    isAnimating = false;
                    animation = AnimationType::None;
                }
                break;
            case AnimationType::Dissappear:
                if (alpha > 0.01)
                {
                    alpha = Interpolate(alpha, 0, dt * 0.001f);
                    if (alpha < 0)
                        alpha = 0;
                }
                else
                {
                    isAnimating = false;
                    animation = AnimationType::None;
                    shouldRemove = true;
                }
                break;
            case AnimationType::Appear:
                if (alpha <1)
                {
                    alpha = Interpolate(alpha, 1, dt * 0.001f);
                    if (alpha > 1)
                        alpha = 1;
                }
                else
                {
                    isAnimating = false;
                    animation = AnimationType::None;
                }
                break;
            default:
                break;
            }
            
        }
    }

    void SGADrawableEntity::render(SGARenderTarget& renderTarget) const 
    {
        renderTarget.drawEntity(position, type, 255*alpha);
    }
}