#include <EntityRenderer.h>
#include <Configuration/RenderConfig.h>
#include <Stratega/Configuration/GameConfig.h>

#include <GridUtils.h>

namespace SGA
{
	void EntityRenderer::init(const GameConfig& gameConfig, const RenderConfig& renderConfig)
	{
		// Initialise atlas
		std::vector<std::string> filePaths;
		for (const auto& namePathPair : renderConfig.entitySpritePaths)
		{
			filePaths.emplace_back(namePathPair.second);
			entitySpritePaths.emplace(gameConfig.getEntityID(namePathPair.first), namePathPair.second);
		}
		atlas.init(filePaths);

		// Initialise vertex array
		vertices.setPrimitiveType(sf::Quads);
	}

	void EntityRenderer::update(const GameState& state)
	{
		// Ensure the vertex array has the correct size
		vertices.resize(4 * state.entities.size());

		for(int i = 0; i < state.entities.size(); i++)
		{
			const auto& entity = state.entities.at(i);
			auto* quadPtr = &vertices[i * 4];
			
			// define the 4 corners
			auto spriteSize = atlas.getSpriteSize();
			auto start = toISO(entity.position.x, entity.position.y) - sf::Vector2f{ spriteSize.x / 4.f, spriteSize.y / 1.4f };
			quadPtr[0].position = sf::Vector2f(start.x, start.y);
			quadPtr[1].position = sf::Vector2f(start.x + spriteSize.x, start.y);
			quadPtr[2].position = sf::Vector2f(start.x + spriteSize.x, start.y + spriteSize.y);
			quadPtr[3].position = sf::Vector2f(start.x, start.y + spriteSize.y);

			// define the 4 texture coordinates
			auto rect = atlas.getSpriteRect(entitySpritePaths.at(entity.typeID));
			quadPtr[0].texCoords = sf::Vector2f(rect.left, rect.top);
			quadPtr[1].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
			quadPtr[2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
			quadPtr[3].texCoords = sf::Vector2f(rect.left, rect.top + rect.height);
		}
	}

	void SGA::EntityRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// apply the tileset texture
		states.texture = &atlas.getAtlasTexture();

		// draw the vertex array
		target.draw(vertices, states);
	}
}
