#pragma once
#include <unordered_map>
#include <Representation/Board.h>
#include <Configuration/IBoardGenerator.h>

namespace SGA
{
	class BoardGenerator : public IBoardGenerator
	{
	public:
		BoardGenerator(const std::vector<std::string>& rowDefinitions, const std::unordered_map<char, TileType>& tileMapping)
			: defaultRowPatterns(rowDefinitions),
			symbolTileMapping(tileMapping)
		{
		}
		
		[[nodiscard]] Board generate(std::mt19937& rngEngine) const override;
		[[nodiscard]] Board generate(const std::vector<std::string>& rowPatterns, std::mt19937& rngEngine) const;
	private:
		std::vector<std::string> defaultRowPatterns;
		std::unordered_map<char, TileType> symbolTileMapping;
	};
}
