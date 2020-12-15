#pragma once
#include <vector>
#include <Representation/Vector2.h>

namespace SGA
{
	struct Entity
	{
		int typeID;
		int id;
		int owner;
		Vector2f position;
		std::vector<int> actionTypeIds;

		std::vector<double> parameters;

		bool shouldRemove=false;//Death?

		//TODO depends on the type...
		bool canExecuteAction;
	};
}