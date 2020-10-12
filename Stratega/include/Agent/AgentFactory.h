#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <Agent/Agent.h>

namespace SGA
{
	class Agent;
	typedef Agent* (*AgentGenerator)();
	
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
		template<typename T>
		bool registerAgent(const std::string& name)
		{
			return registerAgentFn(
				name,
				[]() { return static_cast<Agent*>(new T()); }
			);
		}
		std::unique_ptr<Agent> createAgent(const std::string& name);
		
	private:
		std::unordered_map<std::string, AgentGenerator> agentGeneratorLookup;
		
		AgentFactory() = default;
		~AgentFactory() = default;

		/// <summary>
		/// Returns a factory-reference with all stratega agents registered
		/// Called once to initialize the get() method, do not call more than once!
		/// </summary>
		static AgentFactory& getDefaultFactory();
	};
}
