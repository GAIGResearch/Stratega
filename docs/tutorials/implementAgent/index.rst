.. role:: cpp(code)
   :language: c++

###########################
Implementing a random-agent
###########################

Agents in Stratega are implemented by inheriting from the agent-interface (Stratega/include/Stratega/Agent/Agent.h). The agent-interface provides two methods that we can implement, one for turn-based and one for real-time games.


.. note::
   Work in progress...

.. code-block:: c++

    #pragma once
    #include <Agent/Agent.h>

    namespace SGA
    {
        class MyAgent : public Agent
        {
        public:
            ActionAssignment computeAction(GameState state, const EntityForwardModel& forwardModel, long timeBudgetMs) override;
        };
    }

The computeAction method is called every tick/turn of the agent and it should return the ActionAssignment object with the list of actions the agent wants to execute.
The agent receives a copy of the GameState with fog of war applied, it also receives the forward model object for simulating the game. Depending on which YAML-file was passed to Stratega, the forward model will behave differently.

All we have to do now is fill our computeAction-method with logic. The following example shows how to implement a random-agent(TBS and RTS compatible):

.. code-block:: c++
    :linenos:

    // Generate all available actions
	auto actions = forwardModel.generateActions(state, getPlayerID());
	
    // Uniformly sample a action
	std::uniform_int_distribution<size_t> actionDist(0, actions.size() - 1);
	auto actionIndex = actionDist(getRNGEngine());
	auto action = actions.at(actionIndex);
	
    // Return Action
	return ActionAssignment::fromSingleAction(action);


++++++++++++++++++++
Testing the agent
++++++++++++++++++++
Currently, Stratega doesn't offer a programmatic way to test your agent. Instead, the agent has to be registered in the agent-factory to be specified in a configuration file.

.. note::
    We are aware that the process is quite tedious, and we are currently working on making testing much more straightforward. 

#. Add the files containing the agents source-code to './Stratega/include/Agent/MyAgentFolder'
#. Open './Stratega/src/Agent/AgentFactory.cpp'
#. Add :cpp:`#include <Agent/MyAgentFolder/MyAgent.h>` to the include-list
#. In the function getDefaultFactory add the line :cpp:`factory.registerAgent<MyAgent>("MyAgentName");`
#. To use the agent in a game, modify the list of agents in an configuration file in './gameConfigs' to include MyAgentName
#. Run the GUI with the modified configuration file
