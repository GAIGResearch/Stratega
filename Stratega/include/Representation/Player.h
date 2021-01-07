#pragma once
#include <functional>

namespace SGA
{
	struct GameState;
	struct Entity;
	
	class Player
	{
	public:
		int id;
		double score;
		bool canPlay;
	};
}
