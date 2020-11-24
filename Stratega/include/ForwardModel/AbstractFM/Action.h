#pragma once
#include <variant>
#include <Representation/Vector2.h>

namespace SGA
{
	class Action
	{
		typedef std::variant <int, Vector2f> target;

		int actionTypeID;

		int sourceID;
		
		std::vector<target> targets;		
	};
}