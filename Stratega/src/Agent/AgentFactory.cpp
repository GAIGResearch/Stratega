#include <Stratega/Agent/AgentFactory.h>
#include <Stratega/Agent/RandomAgent.h>
#include <Stratega/Agent/DoNothingAgent.h>
#include <Stratega/Agent/OSLAAgent.h>
#include <Stratega/Agent/TreeSearchAgents/BFSAgent.h>
#include <Stratega/Agent/RHEAAgent/RHEAAgent.h>
#include <Stratega/Agent/TreeSearchAgents/BeamSearchAgent.h>
#include <Stratega/Agent/TreeSearchAgents/DFSAgent.h>
#include <Stratega/Agent/TreeSearchAgents/MCTSAgent.h>
#include <Stratega/Agent/TreeSearchAgents/ActionAbstractionMCTSAgent.h>
#include <Stratega/Agent/PortfolioGreedySearchAgent.h>
#include <Stratega/Agent/POEAgent/POEAgent.h>
#include <Stratega/Agent/PRHEAAgent/PRHEAAgent.h>
#include <Stratega/Agent/MOPRHEAAgent/MOPRHEAAgent.h>
#include <Stratega/Agent/RuleBasedAgents/CombatAgent.h>
#include <Stratega/Agent/ScriptedAgent.h>

namespace SGA
{
	bool AgentFactory::registerAgentFn(const std::string& name, const AgentGenerator& agentFn)
	{
		if (agentGeneratorLookup.find(name) == agentGeneratorLookup.end())
		{
			agentGeneratorLookup.emplace(name, agentFn);
			return true;
		}

		return false;
	}

	bool AgentFactory::registerAgentFn(const std::string& name, const AgentGeneratorParams& agentFn)
	{
		if (agentGeneratorParamsLookup.find(name) == agentGeneratorParamsLookup.end())
		{
			agentGeneratorParamsLookup.emplace(name, agentFn);
			return true;
		}

		return false;
	}

	std::unique_ptr<Agent> AgentFactory::createAgent(const std::string& name)
	{
		auto it = agentGeneratorLookup.find(name);
		if (it == agentGeneratorLookup.end())
		{
			return nullptr;
		}

		return std::unique_ptr<Agent>(it->second());
	}

	std::unique_ptr<Agent> AgentFactory::createAgent(const std::string& name, YAML::Node params)
	{
		auto it = agentGeneratorParamsLookup.find(name);
		if (it == agentGeneratorParamsLookup.end())
		{
			return nullptr;
		}

		return std::unique_ptr<Agent>(it->second(params));
	}

	AgentFactory& AgentFactory::getDefaultFactory()
	{
		static AgentFactory factory;

		// Register agents available in the Stratega framework
		factory.registerAgent<DoNothingAgent>("DoNothingAgent");
		factory.registerAgent<RandomAgent>("RandomAgent");
		factory.registerAgent<BFSAgent>("BFSAgent");
		factory.registerAgent<RHEAAgent>("RHEAAgent");
		factory.registerAgent<OSLAAgent>("OSLAAgent");
		factory.registerAgent<BeamSearchAgent>("BeamSearchAgent");
		factory.registerAgent<DFSAgent>("DFSAgent");
		factory.registerAgent<PortfolioGreedySearchAgent, PortfolioGreedySearchParams>("PGSAgent");
		factory.registerAgent<POEAgent, POEParams>("POEAgent");
		factory.registerAgent<PRHEAAgent, PRHEAParameters>("PRHEAAgent");
		factory.registerAgent<MOPRHEAAgent, MOPRHEAParameters>("MOPRHEAAgent");
		factory.registerAgent<MCTSAgent, MCTSParameters>("MCTSAgent");
		factory.registerAgent<ActionAbstractionMCTSAgent, MCTSParameters>("PMCTSAgent");
		factory.registerAgent<CombatAgent>("CombatAgent");
		factory.registerAgent<ScriptedAgent>("ScriptedAgent");
		
		return factory;
	}

}
