#include <Stratega/NewGUI/SGADrawable.h>

#include <Stratega/NewGUI/SGARenderTarget.h>
namespace SGA
{

    SGADrawableTile::SGADrawableTile(const Vector2f& newPosition, int newZPosition, const TileType& newType)
        : SGADrawable(newPosition, newZPosition), type(newType)
    {
    }

    void SGADrawableTile::update(float /*dt*/)
    {

    }

    void SGADrawableTile::render(SGARenderTarget& renderTarget) const
    {
        if (isHighlighted)
            renderTarget.drawTileHighlight(position, type, 255 * alpha);
        else
            renderTarget.drawTile(position, type, 255 * alpha);
        
    }

    SGADrawableEntity::SGADrawableEntity(const Vector2f& newPosition, int newZPosition, const EntityType& newType, const int newEntityID, const int newPlayerID, float newAlpha)
        : SGADrawable(newPosition, newZPosition), type(newType), entityID(newEntityID), playerID(newPlayerID), animation(AnimationType::None)
    {
        alpha = newAlpha;
    }

    void SGADrawableEntity::update(float dt)
    {
        isAnimating = false;

        if (animation & AnimationType::Move)
        {
            if (targetPosition != position && targetPosition.distance(position) > 0.1f)
            {
                position = Interpolate(position, targetPosition, dt * 10);
                isAnimating = true;
                shouldRemove = false;
            }
            else
            {
                position = targetPosition;
                animation &= ~AnimationType::Move;
            }
        }

        if (animation & AnimationType::Dissappear)
        {
            if (alpha > 0.01)
            {
                alpha = Interpolate(alpha, 0, dt * 2);
                if (alpha < 0)
                    alpha = 0;
                isAnimating = true;
            }
            else
            {
                alpha = 0;
                animation &= ~AnimationType::Dissappear;
                shouldRemove = true;
            }
        }

        if (animation & AnimationType::Appear)
        {
            if (alpha < 1)
            {
                alpha = Interpolate(alpha, 1, dt * 2);
                if (alpha > 0.95)
                    alpha = 1;
                isAnimating = true;
                shouldRemove = false;
            }
            else
            {
                shouldRemove = false;
                animation &= ~AnimationType::Appear;
            }
        }
    }

    void SGADrawableEntity::render(SGARenderTarget& renderTarget) const 
    {
        if(isHighlighted&&!isAnimating)
            renderTarget.drawEntityHighlight(position, type, 255*alpha);
        else
            if (animation & AnimationType::Appear || animation & AnimationType::Dissappear)
            {
                renderTarget.drawEntity(position, type, 255 * alpha);
            }
            else
                renderTarget.drawEntityOutlineColor(position, type, renderTarget.getResourceManager().getPlayerColor(playerID), 255 * alpha);
    }
}