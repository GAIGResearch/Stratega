#include <Stratega/GUI/TileMap.h>
#include <Stratega/GUI/GridUtils.h>
#include <Stratega/Configuration/RenderConfig.h>
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/Representation/GameState.h>

#include "SFML/System/Vector2.hpp"

namespace SGA
{
	void TileMap::init(const GameState& state, const GameConfig& gameConfig, const RenderConfig& renderConfig)
	{
		// Initialise atlas
		std::vector<std::string> filePaths;
		for (const auto& namePathPair : renderConfig.tileSpritePaths)
		{
			filePaths.emplace_back(namePathPair.second);
			tileSpritePaths.emplace(gameConfig.getTileID(namePathPair.first), namePathPair.second);
		}
		tileset.init(filePaths);

		// resize the vertex array to fit the level size
		m_vertices.setPrimitiveType(sf::Quads);
		m_vertices.resize(static_cast<size_t>(state.getBoardWidth() * state.getBoardHeight() * 4));
	}

	void TileMap::update(const GameState& state, const GameState& fowState, bool renderFow, FogRenderType fogType)
	{
		// populate the vertex array, with one quad per tile
		for (int x = 0; x < state.getBoardWidth(); ++x)
		{
			for (int y = 0; y < state.getBoardHeight(); ++y)
			{
				// get the current tile
				const auto& tile = state.getTileAt({ x, y });
				const auto& fogTile = fowState.getTileAt({ x, y });

				// get a pointer to the current tile's quad
				sf::Vertex* quad = &m_vertices[static_cast<size_t>((x + y * state.getBoardWidth()) * 4)];
				if (!renderFow)
				{
					updateTileQuad(quad, tile, sf::Color::White);
				}
				else if (fogType == FogRenderType::Nothing)
				{
					updateTileQuad(quad, tile, fogTile.getTileTypeID() == -1 ? sf::Color::Transparent : sf::Color::White);
				}
				else if (fogType == FogRenderType::Fog)
				{
					updateTileQuad(quad, fogTile, sf::Color::White);
				}
				else if (fogType == FogRenderType::Tiles)
				{
					updateTileQuad(quad, tile, fogTile.getTileTypeID() == -1 ? FOG_COLOR : sf::Color::White);
				}

			}
		}
	}

	void TileMap::updateTileQuad(sf::Vertex* quadPtr, const Tile& tile, const sf::Color& quadColor) const
	{
		// define the 4 corners
		auto start = toISO(tile.x(), tile.y()) - sf::Vector2f{ TILE_ORIGIN_X, TILE_ORIGIN_Y };
		auto tileSize = tileset.getSpriteSize();
		quadPtr[0].position = sf::Vector2f(start.x, start.y);
		quadPtr[1].position = sf::Vector2f(start.x + static_cast<float>(tileSize.x), start.y);
		quadPtr[2].position = sf::Vector2f(start.x + static_cast<float>(tileSize.x), start.y + static_cast<float>(tileSize.y));
		quadPtr[3].position = sf::Vector2f(start.x, start.y + static_cast<float>(tileSize.y));

		// define the 4 texture coordinates
		auto rect = tileset.getSpriteRect(tileSpritePaths.at(tile.getTileTypeID()));
		quadPtr[0].texCoords = sf::Vector2f(rect.left, rect.top);
		quadPtr[1].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
		quadPtr[2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
		quadPtr[3].texCoords = sf::Vector2f(rect.left, rect.top + rect.height);

		// define the 4 colors
		quadPtr[0].color = quadColor;
		quadPtr[1].color = quadColor;
		quadPtr[2].color = quadColor;
		quadPtr[3].color = quadColor;
	}

	void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// apply the tileset texture
		states.texture = &tileset.getAtlasTexture();

		// draw the vertex array
		target.draw(m_vertices, states);
	}
}