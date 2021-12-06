#pragma once
#pragma once
#pragma once
#include <vector>
#include <queue>
#include <map>
#include <Stratega/Representation/Vector2.h>

namespace SGA
{
	class VectorHash2i
	{
	public:

		// Use sum of lengths of first and last names 
		// as hash function. 
		size_t operator()(const Vector2i& v) const
		{
			// Probably not good, but sufficient enough for now
			return static_cast<size_t>(static_cast<float>(v.x) * 11.0f + static_cast<float>(v.y) * 31.0f);
		}
	};
}