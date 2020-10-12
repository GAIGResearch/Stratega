#include <stdexcept>
#include <Configuration/BoardGenerator.h>

namespace SGA
{
	Board BoardGenerator::generate(std::mt19937& rngEngine) const
	{
		return generate(defaultRowPatterns, rngEngine);
	}

	Board BoardGenerator::generate(const std::vector<std::string>& rowPatterns, std::mt19937&) const
	{
		auto height = static_cast<int>(rowPatterns.size());
		auto width = static_cast<int>(rowPatterns[0].size());
		
		Board board(width, height);
		for(auto y = 0; y < height; y++)
		{
			for (auto x = 0; x < width; x++)
			{
				auto tileIt = symbolTileMapping.find(rowPatterns[y][x]);
				if(tileIt == symbolTileMapping.end())
				{
					throw std::runtime_error("Error when generating game board, unknown symbol "/*, rowPatterns[y][x]*/);
				}

				board.setTile(tileIt->second, x, y);
			}
		}

		return board;
	}
}
