#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <Stratega/Agent/Agent.h>
#include <yaml-cpp/yaml.h>

namespace SGA
{
	class Agent;
	typedef Agent* (*AgentGenerator)();
	typedef Agent* (*AgentGeneratorParams)(YAML::Node);
	
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
		bool registerAgent(const std::string& name)
		{
			return registerAgentFn(
				name,
				[]() { return static_cast<Agent*>(new T()); }
			);
		}

		template<typename T, typename Params>
		bool registerAgent(const std::string& name)
		{
			bool normalRegister = registerAgentFn(
				name,
				[]() { return static_cast<Agent*>(new T(Params())); }
			);

			// The agent is already registered without any parameters
			if(!normalRegister)
			{
				return false;
			}

			return registerAgentFn(
				name,
				[](const YAML::Node node) { return static_cast<Agent*>(new T(node.as<Params>())); }
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
