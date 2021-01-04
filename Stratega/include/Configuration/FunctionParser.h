#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <optional>

#include <ForwardModel/FunctionParameter.h>
#include <ForwardModel/FunctionFactory.h>

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
		template<typename Function>
		void parseFunctions(const std::vector<std::string>& functionCalls, std::vector<std::shared_ptr<Function>>& bucket, const IDMap& targetIDs, const IDMap& parameterIDs) const
		{
			for(const auto& code : functionCalls)
			{
				auto abstractFn = parseAbstractFunction(code, targetIDs, parameterIDs);
				if(!abstractFn.has_value())
				{
					throw std::runtime_error("Could not parse '" + code + "'");
				}

				auto instance = FunctionFactory<Function>::get().createFunction(abstractFn->functionName, abstractFn->parameters);
				if (instance == nullptr)
				{
					throw std::runtime_error("Tried calling unknown function " + abstractFn->functionName + ": '" + code + "'");
				}
				bucket.emplace_back(std::move(instance));
			}
		}
		
		// Parses: functionName(param1, param2)
		std::optional<AbstractFunctionCall> parseAbstractFunction(const std::string& code, const IDMap& targetIDs, const IDMap& parameterIDs) const
		{
			// Remove whitespace
			auto copy = code;
			copy.erase(std::remove_if(copy.begin(),copy.end(), [](char x) {return std::isspace(x); }), copy.end());
			std::istringstream ss(copy);
			
			// Parse name
			AbstractFunctionCall call;
			call.functionName = parseText(ss);
			// Not a valid function call
			// ToDo Output error message
			if(ss.peek() != '(')
			{
				return {};
			}
			ss.get(); // Remove '('
			
			// Parse function parameters
			while(ss.peek() != ')' && !ss.eof())
			{
				std::optional<FunctionParameter> param;
				if(((param = parseConstant(ss))) ||
				   ((param = parseParameterReference(ss, targetIDs, parameterIDs))) ||
				   ((param = parseTargetReference(ss, targetIDs))))
				{
					call.parameters.emplace_back(param.value());
				}
				else
				{
					// Unknown function parameter
					// ToDo Output error message
					return {};
				}

				ss.get(); // remove either ) or ,
			}

			return call;
		}

	private:
		
		std::optional<FunctionParameter> parseConstant(std::istringstream& ss) const
		{
			char prefix = '+';
			if(ss.peek() == '+' || ss.peek() == '-')
			{
				prefix = ss.get();
			}
			
			if(std::isdigit(ss.peek()))
			{
				int value;
				ss >> value;
				return FunctionParameter::createConstParameter(static_cast<double>(prefix == '-' ? -value : value));
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
			
			return FunctionParameter::createParameterReference({ parameterIt->second, targetIt->second });
		}

		std::optional<FunctionParameter> parseTargetReference(std::istringstream& ss, const IDMap& targetIDs) const
		{
			std::string targetName = parseText(ss);
			if (targetName.empty())
				return {};
			
			auto targetIt = targetIDs.find(targetName);
			if(targetIt == targetIDs.end())
			{
				throw std::runtime_error("Unknown action target " + targetName);
			}

			return FunctionParameter::createArgumentReference(targetIt->second);
		}

		std::string parseText(std::istringstream& ss) const
		{
			std::string text;
			while(std::isalpha(ss.peek()))
			{
				text.push_back(ss.get());
			}

			return text;
		}
	};
}
