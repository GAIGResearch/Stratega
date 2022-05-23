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

		for (const auto& actionType : config.actionTypes)
		{
			context.actionTypeIDs.emplace(actionType.second.getName(), actionType.first);
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
		for (const auto& buffType : config.buffsTypes)
		{
			context.buffTypeIDs.emplace(buffType.second.getName(), buffType.first);
		}
		return context;
	}

	nonstd::optional<AbstractFunctionCall> FunctionParser::parseAbstractFunction(const std::string& code, const ParseContext& context) const
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
		std::string stringParameter="";
		// Parse function parameters
		while (ss.peek() != ')' && !ss.eof())
		{
			nonstd::optional<FunctionParameter> param;
			if (((param = parseDice(ss, stringParameter, false))) ||
				((param = parseConstant(ss, false))) ||
				((param = parseEntityPlayerReference(ss, context, false))) ||
				((param = parseEntityPlayerParameterReference(ss, context, stringParameter, false))) ||
				((param = parseTimeReference(ss, context, false))) ||
				((param = parseParameterReference(ss, context, stringParameter, false))) ||
				((param = parseTargetReference(ss, context, stringParameter, false))) ||
				((param = parseEntityTypeReference(ss, context, stringParameter, false))) ||
				((param = parseActionTypeReference(ss, context, stringParameter, false))) ||
				((param = parseTileTypeReference(ss, context, false))) ||
				((param = parseBuffTypeReference(ss, context, false))) ||
				((param = parseGameStateParameterReference(ss, context, stringParameter, false))) ||
				((param = parseTechnologyTypeReference(ss, context, stringParameter, false))) ||
				((param = parseExpression(ss, context))))
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

	nonstd::optional<FunctionParameter> FunctionParser::parseConstant(std::istringstream& ss, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		char prefix = '+';
		if (ss.peek() == '+' || ss.peek() == '-')
		{
			ss.get(prefix);
		}

		if (std::isdigit(ss.peek()))
		{
			double value;
			ss >> value;
			if (isalpha(ss.peek()) && (ss.peek() != ')' && ss.peek() != ','))
			{
				ss.seekg(begin);
				return {};
			}
				
			//Check if is not expression
			if (ss.peek() == ')' || ss.peek() == ',')
			{

			}
			else
			{
				if (!allowExpressions)
				{
					ss.seekg(begin);
					return {};
				}
			}

			return FunctionParameter::createConstParameter(prefix == '-' ? -value : value);
		}

		ss.seekg(begin);
		return {};
	}
	
	nonstd::optional<FunctionParameter> FunctionParser::parseDice(std::istringstream& ss, std::string& parameterString, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		/*char prefix = '+';
		if (ss.peek() == '+' || ss.peek() == '-')
		{
			parameterString += ss.peek();
			ss.get(prefix);
		}*/

		int diceNumber = 0;
		int diceFaceNumber = 0;
		//char c = static_cast<char>(ss.peek());
		//Parse dice number
		if (std::isdigit(ss.peek()))
		{
			ss >> diceNumber;
			parameterString += std::to_string(diceNumber);
		}
		else
		{
			parameterString = "";
			ss.seekg(begin);
			return {};
		}

		//Remove d or D
		if (std::isalpha(ss.peek()) && (ss.peek()=='d' || ss.peek()=='D'))
		{
			parameterString += static_cast<char>(ss.peek());
			ss.get();

			if (!std::isdigit(ss.peek())) {
				ss.seekg(begin);
				parameterString = "";
				return{};
			}
				
		}
		else
		{
			ss.seekg(begin);
			parameterString = "";
			return {};
		}

		//Parse dice face number
		if (std::isdigit(ss.peek()))
		{
			ss >> diceFaceNumber;
			parameterString += std::to_string(diceFaceNumber);
		}
		else
		{
			ss.seekg(begin);
			parameterString = "";
			return {};
		}

		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{
			
		}
		else
		{
			if (!allowExpressions)
			{
				ss.seekg(begin);
				parameterString = "";
				return {};
			}
		}
		
		return FunctionParameter::createDiceAnotation({diceNumber ,diceFaceNumber});
	}

	nonstd::optional<FunctionParameter> FunctionParser::parseParameterReference(std::istringstream& ss, const ParseContext& context, std::string& parameterString, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 2, allowExpressions);
		if(!names)
		{
			ss.seekg(begin);
			parameterString = "";
			return {};
		}

		auto targetName = names.value()[0];
		auto parameterName = names.value()[1];
		auto targetIt = context.targetIDs.find(targetName);
		auto parameterIt = context.parameterIDs.find(parameterName);
		if (targetIt == context.targetIDs.end() || parameterIt == context.parameterIDs.end())
		{
			ss.seekg(begin);
			parameterString = "";
			return {};
		}
		parameterString += targetName+"."+ parameterName;
		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{

		}
		else
		{
			if (!allowExpressions)
			{
				ss.seekg(begin);
				parameterString = "";
				return {};
			}
		}

		return FunctionParameter::createParameterReference({ parameterIt->second, targetIt->second });
	}
	
	nonstd::optional<FunctionParameter> FunctionParser::parseExpression(std::istringstream& ss, const ParseContext& context) const
	{
		ExpressionStruct temp;
		//auto begin = ss.tellg();
		std::cout << std::endl;

		//Get expression string
		std::string mathExpress = ss.str();
		mathExpress = mathExpress.substr(mathExpress.find("("));
		
		 
		mathExpress = "";
		while (ss.peek() != EOF && ss.peek() != ',')
		{
			//mathExpress += ss.peek();
			nonstd::optional<FunctionParameter> param;

			std::string parameterString = "";
			
			if (((param = parseDice(ss, parameterString, true))) ||
				((param = parseEntityPlayerParameterReference(ss, context, parameterString, true))) ||
				((param = parseParameterReference(ss, context, parameterString, true))) ||
				((param = parseTechnologyTypeReference(ss, context, parameterString, true))) ||
				((param = parseEntityTypeReference(ss, context, parameterString, true))) ||
				((param = parseActionTypeReference(ss, context, parameterString, true))) ||
				((param = parseTargetReference(ss, context, parameterString, true))) ||
				((param = parseGameStateParameterReference(ss, context, parameterString, true))))
			{				
				mathExpress += parameterString;
				temp.addParameter(param.value(), parameterString);
			}
			else
			{
				char nextChar = static_cast<char>(ss.get());
				if (ss.peek() == EOF && nextChar == ')')
				{
					//If the last character is ) dont add it
				}
				else
					mathExpress += nextChar;						
			}
		}
		temp.setExpression(mathExpress);		
		return FunctionParameter::createExpression(/*ss.str()*/temp);
	}

	nonstd::optional<FunctionParameter> FunctionParser::parseEntityPlayerReference(std::istringstream& ss, const ParseContext& context, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 2);
		if (!names)
		{
			ss.seekg(begin);
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
		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{

		}
		else
		{
			if (!allowExpressions)
			{
				ss.seekg(begin);
				return {};
			}
		}

		return FunctionParameter::createEntityPlayerReference(targetIt->second);
	}

	nonstd::optional<FunctionParameter> FunctionParser::parseTimeReference(std::istringstream& ss, const ParseContext& context, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 2);
		if (!names)
		{
			ss.seekg(begin);
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

		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{

		}
		else
		{
			if (!allowExpressions)
			{
				ss.seekg(begin);
				return {};
			}
		}

		return FunctionParameter::createTimeReference(targetIt->second);
	}

	nonstd::optional<FunctionParameter> FunctionParser::parseEntityPlayerParameterReference(std::istringstream& ss, const ParseContext& context, std::string& parameterString, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 3, allowExpressions);
		if(!names)
		{
			parameterString = "";
			ss.seekg(begin);
			return {};
		}

		auto targetName = names.value()[0];
		auto playerName = names.value()[1];
		auto parameterName = names.value()[2];
		if(playerName != "Player")
		{
			parameterString = "";
			ss.seekg(begin);
			return {};
		}

		auto targetIt = context.targetIDs.find(targetName);
		auto parameterIt = context.parameterIDs.find(parameterName);
		if (targetIt == context.targetIDs.end() || parameterIt == context.parameterIDs.end())
		{
			throw std::runtime_error("Unknown parameter/action-target: " + targetName + ".Player." + parameterName);
		}
		parameterString += targetName + "." + playerName+"." + parameterName;
		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{

		}
		else
		{
			if (!allowExpressions)
			{
				parameterString = "";
				ss.seekg(begin);
				return {};
			}
		}

		return FunctionParameter::createEntityPlayerParameterReference({ parameterIt->second, targetIt->second });
	}
	
	nonstd::optional<FunctionParameter> FunctionParser::parseGameStateParameterReference(std::istringstream& ss, const ParseContext& context, std::string& parameterString, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 2, allowExpressions);
		if(!names)
		{
			parameterString = "";
			ss.seekg(begin);
			return {};
		}

		auto stateName = names.value()[0];
		auto parameterName = names.value()[1];
		if(stateName != "GameState"&& stateName != "Gamestate")
		{
			parameterString = "";
			ss.seekg(begin);
			return {};
		}

		auto parameterIt = context.parameterIDs.find(parameterName);
		if (parameterIt == context.parameterIDs.end())
		{
			throw std::runtime_error("Unknown parameter/action-target: " + stateName + "." + parameterName);
		}
		parameterString += stateName + "." + parameterName;
		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{

		}
		else
		{
			if (!allowExpressions)
			{
				parameterString = "";
				ss.seekg(begin);
				return {};
			}
		}

		return FunctionParameter::createGameStateParameterReference({ parameterIt->second, -1 });
	}

	nonstd::optional<FunctionParameter> FunctionParser::parseTargetReference(std::istringstream& ss, const ParseContext& context, std::string& parameterString, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 1, allowExpressions);
		if (!names)
		{
			ss.seekg(begin);
			parameterString = "";
			return {};
		}

		auto targetIt = context.targetIDs.find(names.value()[0]);
		if (targetIt == context.targetIDs.end())
		{
			ss.seekg(begin); // Set back to start
			parameterString = "";
			return {};
		}
		parameterString = names.value()[0];
		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{

		}
		else
		{
			if (!allowExpressions)
			{
				ss.seekg(begin);
				parameterString = "";
				return {};
			}
		}

		return FunctionParameter::createArgumentReference(targetIt->second);
	}

	nonstd::optional<FunctionParameter> FunctionParser::parseEntityTypeReference(std::istringstream& ss, const ParseContext& context, std::string& parameterString, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 1, allowExpressions);
		if (!names)
		{
			ss.seekg(begin);
			parameterString = "";
			return {};
		}

		auto targetIt = context.entityTypeIDs.find(names.value()[0]);
		if (targetIt == context.entityTypeIDs.end())
		{
			ss.seekg(begin); // Set back to start
			parameterString = "";
			return {};
		}
		parameterString = names.value()[0];
		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{

		}
		else
		{
			if (!allowExpressions)
			{
				ss.seekg(begin);
				parameterString = "";
				return {};
			}
		}

		return FunctionParameter::createEntityTypeReference(targetIt->second);
	}
	

	nonstd::optional<FunctionParameter> FunctionParser::parseActionTypeReference(std::istringstream& ss, const ParseContext& context, std::string& parameterString, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 1, allowExpressions);
		if (!names)
		{
			ss.seekg(begin);
			parameterString = "";
			return {};
		}

		auto targetIt = context.actionTypeIDs.find(names.value()[0]);
		if (targetIt == context.actionTypeIDs.end())
		{
			ss.seekg(begin); // Set back to start
			parameterString = "";
			return {};
		}
		parameterString = names.value()[0];
		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{

		}
		else
		{
			if (!allowExpressions)
			{
				ss.seekg(begin);
				parameterString = "";
				return {};
			}
		}

		return FunctionParameter::createActionTypeReference(targetIt->second);
	}

	nonstd::optional<FunctionParameter> FunctionParser::parseTileTypeReference(std::istringstream& ss, const ParseContext& context, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 1);
		if (!names)
		{
			ss.seekg(begin);
			return {};
		}

		auto targetIt = context.tileTypeIDs.find(names.value()[0]);
		if (targetIt == context.tileTypeIDs.end())
		{
			ss.seekg(begin); // Set back to start
			return {};
		}

		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{

		}
		else
		{
			if (!allowExpressions)
			{
				ss.seekg(begin);
				return {};
			}
		}

		return FunctionParameter::createTileTypeReference(targetIt->second);
	}

	nonstd::optional<FunctionParameter> FunctionParser::parseBuffTypeReference(std::istringstream& ss, const ParseContext& context, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 1);
		if (!names)
		{
			ss.seekg(begin);
			return {};
		}

		auto targetIt = context.buffTypeIDs.find(names.value()[0]);
		if (targetIt == context.buffTypeIDs.end())
		{
			ss.seekg(begin); // Set back to start
			return {};
		}

		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{

		}
		else
		{
			if (!allowExpressions)
			{
				ss.seekg(begin);
				return {};
			}
		}

		return FunctionParameter::createBuffTypeReference(targetIt->second);
	}
	
	nonstd::optional<FunctionParameter> FunctionParser::parseTechnologyTypeReference(std::istringstream& ss, const ParseContext& context, std::string& parameterString, bool allowExpressions) const
	{
		auto begin = ss.tellg();
		auto names = parseAccessorList(ss, 1, allowExpressions);
		if (!names)
		{
			ss.seekg(begin);
			parameterString = "";
			return {};
		}
	
		auto targetIt = context.technologyTypeIDs.find(names.value()[0]);
		if (targetIt == context.technologyTypeIDs.end())
		{
			ss.seekg(begin); // Set back to start
			parameterString = "";
			return {};
		}
		parameterString = names.value()[0];
		char nextCharacter = static_cast<char>(ss.peek());
		//Check if is not expression
		if (nextCharacter == ')' || nextCharacter == ',')
		{

		}
		else
		{
			if (!allowExpressions)
			{
				ss.seekg(begin);
				parameterString = "";
				return {};
			}
		}

		return FunctionParameter::createTechnologyTypeReference(targetIt->second);
	}
	

	nonstd::optional<std::vector<std::string>> FunctionParser::parseAccessorList(std::istringstream& ss, size_t length, bool allowExpressions) const
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
		//char nextChar = ss.peek();
		//char nextNtextChar = ss.peek();
		/*
		if ()
		{
			if (!isalpha(nextNtextChar))
			{
				std::cout<<"test";
			}
			
			if (nextNtextChar != ')')
			{
				std::cout<<"test";
			}
			
		}*/
		if (!lastName.has_value() || ss.peek() == '.'  /*|| (ss.peek()!=')' || ss.peek() != ',')*/)
		{
			ss.seekg(begin);
			return {};
		}
		else if (!allowExpressions && (!isalpha(ss.peek()) && ss.peek() != ')' && ss.peek() != ','))
		{
			ss.seekg(begin);
			return {};
		}
		
		names.emplace_back(lastName.value());
		return names;
	}

	nonstd::optional<std::string> FunctionParser::parseText(std::istringstream& ss) const
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
