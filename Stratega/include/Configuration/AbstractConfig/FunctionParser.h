#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <optional>

#include <ForwardModel/AbstractFM/FunctionParameter.h>

namespace SGA
{
	typedef std::unordered_map<std::string, int> IDMap;
	
	struct AbstractFunctionCall
	{
		std::string functionName;
		std::vector<FunctionParameter> parameters;
	};
	
	class FunctionParser
	{
	public:
		// Parses: functionName(param1, param2)
		std::optional<AbstractFunctionCall> parseFunction(const std::string& code, const IDMap& targetIDs, const IDMap& parameterIDs) const
		{
			// Remove whitespace
			auto copy = code;
			//str2.erase(std::remove_if(str2.begin(),
			//	str2.end(),
			//	[](unsigned char x) {return std::isspace(x); });
			std::istringstream ss(code);
			
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
				   ((param = parseParameterReference(ss, targetIDs, parameterIDs))) ||
				   ((param = parseTargetReference(ss, targetIDs))))
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
		std::optional<FunctionParameter> parseParameterReference(std::istringstream& ss, const IDMap& targetIDs, const IDMap& parameterIDs) const
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
			auto targetIt = targetIDs.find(targetName);
			auto parameterIt = parameterIDs.find(parameterName);
			if(targetIt == targetIDs.end() || parameterIt == parameterIDs.end())
			{
				throw std::runtime_error("Unknown parameter/action-target: " + targetName + "." + parameterName);
			}
			
			return FunctionParameter(parameterIt->second, targetIt->second);
		}

		std::optional<FunctionParameter> parseTargetReference(std::istringstream& ss, const IDMap& targetIDs) const
		{
			std::string targetName = parseText(ss);
			if (targetName.empty() || ss.peek() != ',')
				return {};
			
			auto targetIt = targetIDs.find(targetName);
			if(targetIt == targetIDs.end())
			{
				throw std::runtime_error("Unknown action target " + targetName);
			}

			return FunctionParameter(targetIt->second);
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
