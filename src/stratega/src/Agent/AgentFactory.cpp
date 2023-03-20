#include <Stratega/Agent/AgentFactory.h>
#include <Stratega/Agent/RandomAgent.h>
#include <Stratega/Agent/DoNothingAgent.h>
#include <Stratega/Agent/OSLAAgent.h>
#include <Stratega/Agent/TreeSearchAgents/BFSAgent.h>
#include <Stratega/Agent/RHEAAgent/RHEAAgent.h>
#include <Stratega/Agent/TreeSearchAgents/BeamSearchAgent.h>
#include <Stratega/Agent/TreeSearchAgents/DFSAgent.h>
#include <Stratega/Agent/MCTSAgent/MCTSAgent.h>
#include <Stratega/Agent/NaiveMCTSAgent/NaiveMCTSAgent.h>
#include <Stratega/Agent/SLIAgent/SLIAgent.h>
#include <Stratega/Agent/RuleBasedAgents/CombatAgent.h>
#include <Stratega/Agent/RuleBasedAgents/KTKCombatAgent.h>
#include <Stratega/Agent/RuleBasedAgents/PusherAgent.h>
#include <Stratega/Agent/RuleBasedAgents/BasicTBSAgent.h>
#include <Stratega/Agent/UnitMCTSAgent/UnitMCTSAgent.h>
#include <Stratega/Agent/MinimaxAgent.h>

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
		
		return std::unique_ptr<Agent>(it->second(name));
	}

	std::unique_ptr<Agent> AgentFactory::createAgent(const std::string& name, YAML::Node params)
	{
		auto it = agentGeneratorParamsLookup.find(name);
		if (it == agentGeneratorParamsLookup.end())
		{
			return nullptr;
		}

		return std::unique_ptr<Agent>(it->second(name, params));
	}

	AgentFactory& AgentFactory::getDefaultFactory()
	{
		static AgentFactory factory;

		// Register agents available in the Stratega framework
		factory.registerAgent<DoNothingAgent>("DoNothingAgent");
		factory.registerAgent<RandomAgent>("RandomAgent");
		factory.registerAgent<BFSAgent>("BFSAgent");
		factory.registerAgent<RHEAAgent, RHEAParameters>("RHEAAgent");
		factory.registerAgent<OSLAAgent>("OSLAAgent");
		factory.registerAgent<BeamSearchAgent>("BeamSearchAgent");
		factory.registerAgent<DFSAgent>("DFSAgent");
		factory.registerAgent<MCTSAgent, MCTSParameters>("MCTSAgent");
		factory.registerAgent<NaiveMCTSAgent, NaiveMCTSParameters>("NaiveMCTSAgent");
		factory.registerAgent<SLIAgent, SLIParameters>("SLIAgent");
		factory.registerAgent<CombatAgent>("CombatAgent");
		factory.registerAgent<PusherAgent>("PusherAgent");
        factory.registerAgent<KTKCombatAgent>("KTKCombatAgent");
		factory.registerAgent<UnitMCTSAgent, UnitMCTSParameters>("UnitMCTSAgent");
        factory.registerAgent<MinimaxAgent>("MinimaxAgent");
		factory.registerAgent<BasicTBSAgent>("BasicTBSAgent");
		
		return factory;
	}

}
