#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <optional>

#include <Stratega/ForwardModel/FunctionParameter.h>
#include <Stratega/ForwardModel/FunctionFactory.h>

namespace SGA
{
	struct GameConfig;
	
	struct ParseContext
	{
		typedef std::unordered_map<std::string, int> IDMap;

		IDMap targetIDs;
		IDMap parameterIDs;
		IDMap entityTypeIDs;
		IDMap technologyTypeIDs;

		static ParseContext fromGameConfig(const GameConfig& config);
	};
	
	struct AbstractFunctionCall
	{
		std::string functionName;
		std::vector<FunctionParameter> parameters;
	};
	
	class FunctionParser
	{
	public:
		template<typename Function>
		void parseFunctions(const std::vector<std::string>& functionCalls, std::vector<std::shared_ptr<Function>>& bucket, const ParseContext& context) const
		{
			for(const auto& code : functionCalls)
			{
				auto abstractFn = parseAbstractFunction(code, context);
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
		std::optional<AbstractFunctionCall> parseAbstractFunction(const std::string& code, const ParseContext& context) const;

	private:

		std::optional<FunctionParameter> parseConstant(std::istringstream& ss) const;
		std::optional<FunctionParameter> parseParameterReference(std::istringstream& ss, const ParseContext& context) const;
		std::optional<FunctionParameter> parseEntityPlayerParameterReference(std::istringstream& ss, const ParseContext& context) const;
		std::optional<FunctionParameter> parseTargetReference(std::istringstream& ss, const ParseContext& context) const;
		std::optional<FunctionParameter> parseEntityPlayerReference(std::istringstream& ss, const ParseContext& context) const;
		std::optional<FunctionParameter> parseTimeReference(std::istringstream& ss, const ParseContext& context) const;
		std::optional<FunctionParameter> parseEntityTypeReference(std::istringstream& ss, const ParseContext& context) const;
		std::optional<FunctionParameter> parseTechnologyTypeReference(std::istringstream& ss, const ParseContext& context) const;
		/// <summary>
		/// Parses constructs in the form of Name1.Name2.Name3 and returns the names
		/// </summary>
		std::optional<std::vector<std::string>> parseAccessorList(std::istringstream& ss, size_t length) const;
		std::optional<std::string> parseText(std::istringstream& ss) const;
	};
}
