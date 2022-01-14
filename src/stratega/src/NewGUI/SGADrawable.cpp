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
        if (isHighlighted)
            renderTarget.drawTileHighlight(position, type, 255 * alpha);
        else
            renderTarget.drawTile(position, type, 255 * alpha);
        
    }

    SGADrawableEntity::SGADrawableEntity(const Vector2f& newPosition, int newZPosition, const EntityType& newType, const int newEntityID, const int newPlayerID)
        : SGADrawable(newPosition, newZPosition), type(newType), entityID(newEntityID), playerID(newPlayerID), animation(AnimationType::None)
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
                    position = Interpolate(position, targetPosition, dt * 10);
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
                    alpha = Interpolate(alpha, 0, dt * 5);
                    if (alpha < 0)
                        alpha = 0;
                }
                else
                {
                    alpha = 0;
                    isAnimating = false;
                    animation = AnimationType::None;
                    shouldRemove = true;
                }
                break;
            case AnimationType::Appear:
                if (alpha <1)
                {
                    alpha = Interpolate(alpha, 1, dt * 5);
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
        if(isHighlighted)
            renderTarget.drawEntityHighlight(position, type, 255*alpha);
        else
            renderTarget.drawEntityOutlineColor(position, type, renderTarget.getResourceManager().getPlayerColor(playerID), 255 * alpha);
    }
}