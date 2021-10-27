#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <Stratega/Agent/Agent.h>
#include <yaml-cpp/yaml.h>

namespace SGA
{
	class Agent;
	typedef Agent* (*AgentGenerator)(std::string);
	typedef Agent* (*AgentGeneratorParams)(std::string,YAML::Node);
	
	class AgentFactory
	{
	public:
		static AgentFactory& get()
		{
			static AgentFactory& instance = getDefaultFactory();
			return instance;
		}

		// This class should not be copied
		AgentFactory(const AgentFactory&) = delete;
		AgentFactory(AgentFactory&&) = delete;
		AgentFactory& operator=(const AgentFactory& other) = delete;
		AgentFactory& operator=(AgentFactory&& other) noexcept = delete;
		
		bool registerAgentFn(const std::string& name, const AgentGenerator& agentFn);
		bool registerAgentFn(const std::string& name, const AgentGeneratorParams& agentFn);
		
		template<typename T>
		bool registerAgent(const std::string newName)
		{
			return registerAgentFn(
				newName,
				[](std::string registeredName) { return static_cast<Agent*>(new T(registeredName)); }
			);
		}

		template<typename T, typename Params>
		bool registerAgent(const std::string& newName)
		{
			bool normalRegister = registerAgentFn(
				newName,
				[](std::string agentName) { return static_cast<Agent*>(new T(agentName,Params())); }
			);

			// The agent is already registered without any parameters
			if(!normalRegister)
			{
				return false;
			}

			return registerAgentFn(
				newName,
				[](std::string registeredName,const YAML::Node node) { return static_cast<Agent*>(new T(registeredName, node.as<Params>())); }
			);
		}
		
		std::unique_ptr<Agent> createAgent(const std::string& name);
		std::unique_ptr<Agent> createAgent(const std::string& name, YAML::Node params);
		
	private:
		std::unordered_map<std::string, AgentGenerator> agentGeneratorLookup;
		std::unordered_map<std::string, AgentGeneratorParams> agentGeneratorParamsLookup;
		
		AgentFactory() = default;
		~AgentFactory() = default;

		/// <summary>
		/// Returns a factory-reference with all stratega agents registered
		/// Called once to initialize the get() method, do not call more than once!
		/// </summary>
		static AgentFactory& getDefaultFactory();
	};
}
