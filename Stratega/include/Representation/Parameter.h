#pragma once
#include <string>

namespace SGA
{
	typedef int ParameterID;
	
	struct Parameter
	{
		ParameterID id;
		std::string name;
		int index;

		double defaultValue;
		double minValue;
		double maxValue;
	};
}