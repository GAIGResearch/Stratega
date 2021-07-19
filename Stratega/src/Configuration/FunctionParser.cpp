#include <Stratega/Configuration/FunctionParser.h>
#include <Stratega/Configuration/GameConfig.h>

namespace SGA
{

	ParseContext ParseContext::fromGameConfig(const GameConfig& config)
	{
		ParseContext context;
		context.parameterIDs = config.parameters;
		for (const auto& entityType : config.entityTypes)
		{
			context.entityTypeIDs.emplace(entityType.second.getName(), entityType.first);
		}

		for (const auto& technologyTreeType : config.technologyTreeCollection.technologyTreeTypes)
		{
			for (const auto& technology : technologyTreeType.second.technologies)
			{
				context.technologyTypeIDs.emplace(technology.second.name, technology.first);
			}
			
		}

		for (const auto& tileType : config.tileTypes)
		{
			context.tileTypeIDs.emplace(tileType.second.getName(), tileType.first);
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
		if(auto functionName = parseText(ss))
		{
			call.functionName = functionName.value();
		}
		
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
				((param = parseEntityPlayerReference(ss, context))) ||
				((param = parseEntityPlayerParameterReference(ss, context))) ||
				((param = parseTimeReference(ss, context))) ||
				((param = parseParameterReference(ss, context))) ||
				((param = parseTargetReference(ss, context))) ||
				((param = parseEntityTypeReference(ss, context))) ||
				((param = parseTileTypeReference(ss, context))) ||
				((param = parseTechnologyTypeReference(ss, context))))
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
			ss.get(prefix);
		}

		if (std::isdigit(ss.peek()))
		{
			double value;
			ss >> value;
			return FunctionParameter::createConstParameter(static_cast<double>(prefix == '-' ? -value : value));
		}

		return {};
	}

	std::optional<FunctionParameter> FunctionParser::parseParameterReference(std::istringstream& ss, const ParseContext& context) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 2);
		if(!names)
		{
			ss.seekg(begin);
			return {};
		}

		auto targetName = names.value()[0];
		auto parameterName = names.value()[1];
		auto targetIt = context.targetIDs.find(targetName);
		auto parameterIt = context.parameterIDs.find(parameterName);
		if (targetIt == context.targetIDs.end() || parameterIt == context.parameterIDs.end())
		{
			ss.seekg(begin);
			return {};
		}

		return FunctionParameter::createParameterReference({ parameterIt->second, targetIt->second });
	}

	std::optional<FunctionParameter> FunctionParser::parseEntityPlayerReference(std::istringstream& ss, const ParseContext& context) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 2);
		if (!names)
		{
			return {};
		}

		auto targetName = names.value()[0];
		auto playerName = names.value()[1];
		if (playerName != "Player")
		{
			ss.seekg(begin);
			return {};
		}

		auto targetIt = context.targetIDs.find(targetName);
		if (targetIt == context.targetIDs.end())
		{
			throw std::runtime_error("Unknown action-target: " + targetName);
		}

		return FunctionParameter::createEntityPlayerReference(targetIt->second);
	}

	std::optional<FunctionParameter> FunctionParser::parseTimeReference(std::istringstream& ss, const ParseContext& context) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 2);
		if (!names)
		{
			return {};
		}

		auto targetName = names.value()[0];
		auto timeName = names.value()[1];
		if (timeName != "Time")
		{
			ss.seekg(begin);
			return {};
		}
		
		auto targetIt = context.targetIDs.find(targetName);
		if (targetIt == context.targetIDs.end())
		{
			throw std::runtime_error("Unknown action-target: " + targetName);
		}

		return FunctionParameter::createTimeReference(targetIt->second);
	}

	std::optional<FunctionParameter> FunctionParser::parseEntityPlayerParameterReference(std::istringstream& ss, const ParseContext& context) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 3);
		if(!names)
		{
			return {};
		}

		auto targetName = names.value()[0];
		auto playerName = names.value()[1];
		auto parameterName = names.value()[2];
		if(playerName != "Player")
		{
			ss.seekg(begin);
			return {};
		}

		auto targetIt = context.targetIDs.find(targetName);
		auto parameterIt = context.parameterIDs.find(parameterName);
		if (targetIt == context.targetIDs.end() || parameterIt == context.parameterIDs.end())
		{
			throw std::runtime_error("Unknown parameter/action-target: " + targetName + ".Player." + parameterName);
		}

		return FunctionParameter::createEntityPlayerParameterReference({ parameterIt->second, targetIt->second });
	}

	std::optional<FunctionParameter> FunctionParser::parseTargetReference(std::istringstream& ss, const ParseContext& context) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 1);
		if (!names)
		{
			return {};
		}

		auto targetIt = context.targetIDs.find(names.value()[0]);
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
		auto names = parseAccessorList(ss, 1);
		if (!names)
		{
			return {};
		}

		auto targetIt = context.entityTypeIDs.find(names.value()[0]);
		if (targetIt == context.entityTypeIDs.end())
		{
			ss.seekg(begin); // Set back to start
			return {};
		}
		
		return FunctionParameter::createEntityTypeReference(targetIt->second);
	}

	std::optional<FunctionParameter> FunctionParser::parseTileTypeReference(std::istringstream& ss, const ParseContext& context) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 1);
		if (!names)
		{
			return {};
		}

		auto targetIt = context.tileTypeIDs.find(names.value()[0]);
		if (targetIt == context.tileTypeIDs.end())
		{
			ss.seekg(begin); // Set back to start
			return {};
		}

		return FunctionParameter::createTileTypeReference(targetIt->second);
	}
	
	std::optional<FunctionParameter> FunctionParser::parseTechnologyTypeReference(std::istringstream& ss, const ParseContext& context) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 1);
		if (!names)
		{
			return {};
		}
	
		auto targetIt = context.technologyTypeIDs.find(names.value()[0]);
		if (targetIt == context.technologyTypeIDs.end())
		{
			ss.seekg(begin); // Set back to start
			return {};
		}

		return FunctionParameter::createTechnologyTypeReference(targetIt->second);
	}
	

	std::optional<std::vector<std::string>> FunctionParser::parseAccessorList(std::istringstream& ss, size_t length) const
	{
		auto begin = ss.tellg();
		
		std::vector<std::string> names;
		while(names.size() < length - 1) // Parse until the last dot
		{
			auto targetName = parseText(ss);
			if (!targetName.has_value())
			{
				ss.seekg(begin);
				return {};
			}
			
			names.emplace_back(targetName.value());
			if(ss.peek() != '.')
			{
				break;
			}
			ss.get();
		}

		auto lastName = parseText(ss);
		if (!lastName.has_value() || ss.peek() == '.')
		{
			ss.seekg(begin);
			return {};
		}
		
		names.emplace_back(lastName.value());
		return names;
	}

	std::optional<std::string> FunctionParser::parseText(std::istringstream& ss) const
	{
		auto begin = ss.tellg();
		
		std::string text;
		while (std::isalpha(ss.peek()))
		{
			text.push_back(static_cast<char>(ss.get()));
		}

		if (text.empty())
		{
			ss.seekg(begin);
			return {};
		}
		
		return text;
	}
}
