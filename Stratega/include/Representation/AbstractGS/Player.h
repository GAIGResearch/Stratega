#pragma once

namespace SGA
{
	struct Player
	{
		Player(int id):
		id(id),
		score(0),
		canPlay(true)
		{
			
		}
		int id;
		double score;
		bool canPlay;
	};
}