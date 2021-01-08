#pragma once
#include <functional>

namespace SGA
{
	struct GameState;
	struct Entity;
	
	class Player
	{
	public:
		static const int NEUTRAL_PLAYER_ID = -1;
		
		int id;
		double score;
		bool canPlay;
	};
}
