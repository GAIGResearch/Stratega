#pragma once
#include <string>
#include <vector>

#include <ForwardModel/AbstractFM/FunctionParameter.h>

namespace SGA
{
	class AbstractFunctionCall
	{
		std::string functionName;
		std::vector<FunctionParameter> parameters;
	};
	
	class FunctionParser
	{
	public:
		AbstractFunctionCall parse(const std::string& code)
		{
			
		}
	};
}
