#pragma once
#include <vector>
#include <Representation/Vector2.h>

namespace SGA
{
	template<typename Type>
	class Grid2D
	{
		size_t width;
		size_t height;
		std::vector<Type> grid;

	public:
		typedef typename std::vector<Type>::reference reference;
		typedef typename std::vector<Type>::const_reference const_reference;

		template<typename InputIterator>
		Grid2D(size_t width, InputIterator begin, InputIterator end)
			: width(width), height(-1), grid(begin, end)
		{
			if(grid.size() % width != 0)
			{
				throw std::runtime_error("Received a amount of values that is not a multiple of width.");
			}

			height = grid.size() / width;
		}

		
		Grid2D(size_t width, size_t height, Type defaultValue = Type())
			: width(width), height(height), grid(width * height, defaultValue)
		{
		}

		reference operator[] (const Vector2i& pos) { return grid[pos.y * width + pos.x]; }
		const_reference operator[] (const Vector2i& pos) const { return grid[pos.y * width + pos.x]; }

		reference get(int x, int y) { return grid[y * width + x]; }
		const_reference get(int x, int y) const { return grid[y * width + x]; }

		int getWidth() const { return width; }
		int getHeight() const { return height; }
		
		bool isInBounds(const Vector2i& pos) const { return pos.x > 0 && pos.x < width&& pos.y > 0 && pos.y < height; };
		bool isInBounds(int x, int y) const { return isInBounds({ x, y }); };
	};
}