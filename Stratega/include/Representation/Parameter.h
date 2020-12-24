#pragma once
#include <string>

namespace SGA
{
	struct Parameter
	{
		std::string name;
		int index;

		double defaultValue;
		double minValue;
		double maxValue;
	};
}