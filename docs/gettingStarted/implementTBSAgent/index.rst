.. role:: cpp(code)
   :language: c++

################################
Implementing a TBS random-agent
################################

Agents in Stratega are implemented by inheriting from the agent-interface. The agent-interface provides two methods that we can implement, one for turn-based and one for real-time games.

.. code-block:: c++

    #pragma once
    #include <Agent/Agent.h>

    namespace SGA
    {
        class MyAgent : public Agent
        {
        public:
            void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
        };
    }

The run-method is called once at the start of a game and should run until it is over.
The game-communicator can be used to get a copy of the current gamestate and to execute actions.
Additionally, the agent receives the forward model for simulating the game. Depending on which YAML-file was passed to Stratega, the forward model will behave differently. 

All we have to do now is fill our run-method with logic. The following example shows how to implement a random-agent:

.. code-block:: c++
    :linenos:

    while (!gameCommunicator.isGameOver())
    {
        if (gameCommunicator.isMyTurn())
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

Note in line 8 we have to tell the forward model for which player we want to generate the actions; That is because the forward model still allows you to generate actions for players even if it's not their turn.

To sample a action in line 11, we ask the game-communicator for our random number generator. Although you can use other methods to generate random numbers, it may make your agent nondeterministic.

Lastly, in line 14, we send our action to the game-runner to be executed. Since every agent in Stratega runs in its own thread, it might happen that we send the action, and immediately fetch the same gamestate since the game-runner hasn't updated it yet. Because of this, executeAction will not return control until the given action is executed.


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
#. Modify the list of agents in an configuration file in './gameConfigs' to include MyAgentName
#. Run the GUI with the modified configuration file