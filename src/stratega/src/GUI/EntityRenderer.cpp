#include <Stratega/GUI/EntityRenderer.h>
#include <Stratega/Configuration/RenderConfig.h>
#include <Stratega/Configuration/GameConfig.h>

#include <Stratega/GUI/GridUtils.h>

namespace SGA
{
	EntityRenderer::EntityRenderer()
		: atlas(2)
	{
		
	}

	
	void EntityRenderer::init(const GameState& /*initialState*/, const GameConfig& gameConfig, const RenderConfig& renderConfig)
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

		// Initialise shader
		outlineShader.loadFromFile(renderConfig.outlineShaderPath, sf::Shader::Fragment);
		outlineShader.setUniform("texture", sf::Shader::CurrentTexture);
		outlineShader.setUniform("outlineThickness", 1.4f);
		outlineShader.setUniform("textureSize", sf::Glsl::Vec2(atlas.getAtlasTexture().getSize()));
	}

	void EntityRenderer::update(const GameState& state)
	{
		auto& entities = state.getEntities();

		// Ensure the vertex array has the correct size
		vertices.resize(4 * entities.size());

		int i = 0;
		for(auto entity = entities.begin(); entity != entities.end(); entity++)
		{
			//const auto& entity = state.getEntities()[i];
			auto* quadPtr = &vertices[static_cast<size_t>(i * 4)];
			
			// define the 4 corners
			auto spriteSize = atlas.getSpriteSize();
			auto start = toISO(entity->x(), entity->y()) - sf::Vector2f{(static_cast<float>(spriteSize.x) / 4.f),(static_cast<float>(spriteSize.y) / 1.4f )};
			quadPtr[0].position = sf::Vector2f(start.x, start.y);
			quadPtr[1].position = sf::Vector2f(start.x + static_cast<float>(spriteSize.x), start.y);
			quadPtr[2].position = sf::Vector2f(start.x + static_cast<float>(spriteSize.x), start.y + static_cast<float>(spriteSize.y));
			quadPtr[3].position = sf::Vector2f(start.x, start.y + static_cast<float>(spriteSize.y));

			// define the 4 texture coordinates
			auto rect = atlas.getSpriteRect(entitySpritePaths.at(entity->getEntityTypeID()));
			quadPtr[0].texCoords = sf::Vector2f(rect.left, rect.top);
			quadPtr[1].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
			quadPtr[2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
			quadPtr[3].texCoords = sf::Vector2f(rect.left, rect.top + rect.height);

			quadPtr[0].color = entity->isNeutral() ? sf::Color::Transparent : PLAYER_COLORS[static_cast<size_t>(entity->getOwnerID())];
			quadPtr[1].color = entity->isNeutral() ? sf::Color::Transparent : PLAYER_COLORS[static_cast<size_t>(entity->getOwnerID())];
			quadPtr[2].color = entity->isNeutral() ? sf::Color::Transparent : PLAYER_COLORS[static_cast<size_t>(entity->getOwnerID())];
			quadPtr[3].color = entity->isNeutral() ? sf::Color::Transparent : PLAYER_COLORS[static_cast<size_t>(entity->getOwnerID())];
			i++;
		}
	}

	void EntityRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();
		
		// apply the tileset texture
		states.texture = &atlas.getAtlasTexture();
		states.shader = &outlineShader;
		
		// draw the vertex array
		target.draw(vertices, states);
	}
}
