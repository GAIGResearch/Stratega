#include <Stratega/NewGUI/SGADrawable.h>

#include <Stratega/NewGUI/SGARenderTarget.h>
namespace SGA
{

    SGADrawable::SGADrawable(const Vector2i& newPosition, int newZPosition, const TileType& newType)
        : position(newPosition), zPosition(newZPosition), type(newType)
    {
    }

    void SGADrawable::update()
    {

    }

    void SGADrawable::render(SGARenderTarget& renderTarget)
    {
        renderTarget.drawTile(position, type);
    }
}