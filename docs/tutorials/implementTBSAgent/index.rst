.. role:: cpp(code)
   :language: c++

################################
Implementing a RTS random-agent
################################

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
            void runTBS(AgentGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
            void runRTS(AgentGameCommunicator& gameCommunicator, RTSForwardModel forwardModel) override;
        };
    }

The run methods are called once at the start of the game and should not return before the game is over. The game communicator object can be used to query elements from the game state, create copies of it and to indicate what actions must be executed in the real game. Additionally, the agent receives the forward model object for simulating the game. Depending on which YAML-file was passed to Stratega, the forward model will behave differently.

All we have to do now is fill our run-method with logic. The following example shows how to implement a random-agent:

.. code-block:: c++
    :linenos:

    while (!gameCommunicator.isGameOver())
    {
        if (gameCommunicator.isMyTurn())
        {
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> deltaTime = now - lastExecution;
		if (deltaTime.count() >= 1)
		{
        	        // Fetch state
                	auto state = gameCommunicator.getGameState();
                	
			// Generate all available actions
                	auto actions = forwardModel.generateActions(state, gameCommunicator.getPlayerID());
                	
			// Uniformly sample a action
                	std::uniform_int_distribution<int> actionDist(0, actions.size() - 1);
                	auto actionIndex = actionDist(gameCommunicator.getRNGEngine());
                	auto action = actions.at(actionIndex);
               		
			// Send action to the game-runner
                	gameCommunicator.executeAction(action);
            }
        }
    }



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
