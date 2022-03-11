#pragma once
#include <vector>
#include <Stratega/ForwardModel/ActionTarget.h>
#include <Stratega/Representation/EntityType.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/TechnologyTree.h>
#include <Stratega/Representation/TileType.h>
#include <Stratega/Representation/BuffType.h>
#include <boost/variant.hpp>
#include <regex>
namespace SGA
{
	//struct ExpressionStruct
	//struct GameState;
	
	class FunctionParameter;
	
	struct ExpressionStruct
	{
		std::unordered_map<std::string, std::shared_ptr<FunctionParameter>> variable;
		std::string expression;

		void addParameter(FunctionParameter parameter, std::string newVariable);

		void setExpression(std::string string);

		std::string getExpression(const GameState& state, const std::vector<ActionTarget>& actionTargets);

		std::unordered_map<ParameterID, std::string> getExpressionCost(const GameState& state, const std::vector<ActionTarget>& actionTargets);

		~ExpressionStruct()
		{
			expression.~basic_string();
		}
	};	
}
