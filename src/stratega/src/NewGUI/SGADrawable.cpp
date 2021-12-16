#include <Stratega/NewGUI/SGADrawable.h>

#include <Stratega/NewGUI/SGARenderTarget.h>
namespace SGA
{

    SGADrawableTile::SGADrawableTile(const Vector2f& newPosition, int newZPosition, const TileType& newType)
        : SGADrawable(newPosition, newZPosition), type(newType)
    {
    }

    void SGADrawableTile::update()
    {

    }

    void SGADrawableTile::render(SGARenderTarget& renderTarget) const
    {
        renderTarget.drawTile(position, type);
    }

    SGADrawableEntity::SGADrawableEntity(const Vector2f& newPosition, int newZPosition, const EntityType& newType)
        : SGADrawable(newPosition, newZPosition), type(newType)
    {
    }

    void SGADrawableEntity::update()
    {

    }

    void SGADrawableEntity::render(SGARenderTarget& renderTarget) const 
    {
        renderTarget.drawEntity(position, type);
    }
}