#pragma once
#include <string>

namespace SGA
{
	typedef int ParameterID;
	
	struct Parameter
	{
		ParameterID id = -1;
		std::string name;
		int index = -1;

		double defaultValue = 0;
		double minValue = 0;
		double maxValue = 0;
	};
}