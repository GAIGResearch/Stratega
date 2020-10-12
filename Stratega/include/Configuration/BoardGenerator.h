#pragma once
#include <unordered_map>
#include <Representation/Board.h>
#include <Configuration/IBoardGenerator.h>

namespace SGA
{
	class BoardGenerator : public IBoardGenerator
	{
	public:
		[[nodiscard]] Board generate(std::mt19937& rngEngine) const override;
		[[nodiscard]] Board generate(const std::vector<std::string>& rowPatterns, std::mt19937& rngEngine) const;

		void setDefaultRowPatterns(std::vector<std::string>&& patterns)
		{
			defaultRowPatterns = std::move(patterns);
		}

		void addTile(char symbol, const TileType& tile)
		{
			symbolTileMapping.emplace(symbol, tile);
		}
	private:
		std::vector<std::string> defaultRowPatterns;
		std::unordered_map<char, TileType> symbolTileMapping;
	};
}
