#pragma once
#include <Representation/Parameter.h>
#include <unordered_map>

namespace SGA
{
	struct Player
	{
		static const int NEUTRAL_PLAYER_ID = -1;
		
		int id;
		double score;
		bool canPlay;
		std::vector<double> parameters;
	};
}
