#include <Stratega/NewGUI/SGARenderTarget.h>

namespace SGA
{
	SGARenderTarget::SGARenderTarget(sf::RenderTarget& newTarget/*, const ResourceManager& resourceManager, World& world*/) :
		target(newTarget)/*, resourceManager(resourceManager), world(world)*/
	{

	}

	void SGARenderTarget::drawTile(const Vector2i& position, const TileType& tile)
	{
		std::cout<<"RenderingTile"<<std::endl;
	}

	void SGARenderTarget::drawVertices(sf::VertexArray vertexArray)
	{
		target.draw(vertexArray);
	}
}