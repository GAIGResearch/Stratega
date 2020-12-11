#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <optional>

#include <ForwardModel/AbstractFM/FunctionParameter.h>

namespace SGA
{
	struct AbstractFunctionCall
	{
		std::string functionName;
		std::vector<FunctionParameter> parameters;
	};
	
	class FunctionParser
	{
	public:
		// Parses: functionName(param1, param2)
		std::optional<AbstractFunctionCall> parseFunction(std::istringstream& ss) const
		{
			// Parse name
			AbstractFunctionCall call;
			call.functionName = parseText(ss);
			// Not a valid function call
			// ToDo Output error message
			if(ss.peek() != '(')
			{
				return {};
			}

			// Parse function parameters
			while(!ss.eof() && ss.peek() != ')')
			{
				ss.get(); // remove character either ) or ,
				
				std::optional<FunctionParameter> param;
				if(((param = parseConstant(ss))) ||
				   ((param = parseParameterReference(ss))) ||
				   ((param = parseTargetReference(ss))))
				{
					call.parameters.emplace_back(param.value());
					if(ss.peek() != ',')
					{
						return {};
					}
				}
				else
				{
					// Unknown function parameter
					// ToDo Output error message
					return {};
				}
			}

			return call;
		}

	private:
		
		std::optional<FunctionParameter> parseConstant(std::istringstream& ss) const
		{
			if(std::isdigit(ss.peek()))
			{
				int value;
				ss >> value;
				return FunctionParameter(static_cast<double>(value));
			}

			return {};
		}

		// targetName.parameterName
		std::optional<FunctionParameter> parseParameterReference(std::istringstream& ss) const
		{
			auto begin = ss.tellg();
			auto targetName = parseText(ss);
			if(ss.peek() != '.')
			{
				ss.seekg(begin); // Set back to start
				return {};
			}

			ss.get();
			auto parameterName = parseText(ss);
			return FunctionParameter(0);
		}

		std::optional<FunctionParameter> parseTargetReference(std::istringstream& ss) const
		{
			std::string targetName = parseText(ss);
			return FunctionParameter(0);
		}

		std::string parseText(std::istringstream& ss) const
		{
			auto test = ss.peek();
			std::string text;
			while(std::isalpha(ss.peek()))
			{
				text.push_back(ss.get());
			}

			return text;
		}
	};
}
