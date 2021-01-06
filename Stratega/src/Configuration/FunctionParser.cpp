#include <Configuration/FunctionParser.h>
#include <Configuration/GameConfig.h>

namespace SGA
{

	ParseContext ParseContext::fromGameConfig(const GameConfig& config)
	{
		ParseContext context;
		context.parameterIDs = config.parameters;
		for (const auto& entityType : config.entityTypes)
		{
			context.entityTypeIDs.emplace(entityType.second.name, entityType.first);
		}

		return context;
	}

	std::optional<AbstractFunctionCall> FunctionParser::parseAbstractFunction(const std::string& code, const ParseContext& context) const
	{
		// Remove whitespace
		auto copy = code;
		copy.erase(std::remove_if(copy.begin(), copy.end(), [](char x) { return std::isspace(x); }), copy.end());
		std::istringstream ss(copy);

		// Parse name
		AbstractFunctionCall call;
		call.functionName = parseText(ss);
		// Not a valid function call
		// ToDo Output error message
		if (ss.peek() != '(')
		{
			return {};
		}
		ss.get(); // Remove '('

		// Parse function parameters
		while (ss.peek() != ')' && !ss.eof())
		{
			std::optional<FunctionParameter> param;
			if (((param = parseConstant(ss))) ||
				((param = parseParameterReference(ss, context))) ||
				((param = parseTargetReference(ss, context))) ||
				((param = parseEntityTypeReference(ss, context))))
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

	std::optional<FunctionParameter> FunctionParser::parseConstant(std::istringstream& ss) const
	{
		char prefix = '+';
		if (ss.peek() == '+' || ss.peek() == '-')
		{
			prefix = ss.get();
		}

		if (std::isdigit(ss.peek()))
		{
			int value;
			ss >> value;
			return FunctionParameter::createConstParameter(static_cast<double>(prefix == '-' ? -value : value));
		}

		return {};
	}

	std::optional<FunctionParameter> FunctionParser::parseParameterReference(std::istringstream& ss, const ParseContext& context) const
	{
		auto begin = ss.tellg();
		auto targetName = parseText(ss);
		if (ss.peek() != '.')
		{
			ss.seekg(begin); // Set back to start
			return {};
		}

		ss.get();
		auto parameterName = parseText(ss);
		auto targetIt = context.targetIDs.find(targetName);
		auto parameterIt = context.parameterIDs.find(parameterName);
		if (targetIt == context.targetIDs.end() || parameterIt == context.parameterIDs.end())
		{
			throw std::runtime_error("Unknown parameter/action-target: " + targetName + "." + parameterName);
		}

		return FunctionParameter::createParameterReference({ parameterIt->second, targetIt->second });
	}

	std::optional<FunctionParameter> FunctionParser::parseTargetReference(std::istringstream& ss, const ParseContext& context) const
	{
		auto begin = ss.tellg();
		std::string targetName = parseText(ss);
		if (targetName.empty())
		{
			ss.seekg(begin); // Set back to start
			return {};
		}

		auto targetIt = context.targetIDs.find(targetName);
		if (targetIt == context.targetIDs.end())
		{
			ss.seekg(begin); // Set back to start
			return {};
		}

		return FunctionParameter::createArgumentReference(targetIt->second);
	}

	std::optional<FunctionParameter> FunctionParser::parseEntityTypeReference(std::istringstream& ss, const ParseContext& context) const
	{
		auto begin = ss.tellg();
		std::string typeName = parseText(ss);
		if (typeName.empty())
		{
			ss.seekg(begin); // Set back to start
			return {};
		}

		auto targetIt = context.entityTypeIDs.find(typeName);
		if (targetIt == context.entityTypeIDs.end())
		{
			throw std::runtime_error("Unknown entity type" + typeName);
		}

		return FunctionParameter::createEntityTypeReference(targetIt->second);
	}

	std::string FunctionParser::parseText(std::istringstream& ss) const
	{
		std::string text;
		while (std::isalpha(ss.peek()))
		{
			text.push_back(ss.get());
		}

		return text;
	}
}
