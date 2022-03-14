.. _implement_agent:

#############################
Implementing Simple AI Agents
#############################

This page shows a few examples about how to create C++ AI agents for Stratega. In all cases, a new AI agent must do the following, independently if the agent is created to play RTS or TBS games:

#. Extend the class **Agent** from `./include/Stratega/Agent/Agent.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Agent/Agent.h>`_
#. Override the method *computeAction*.   

The **computeAction** method is called every tick/turn of the game and it should return an ActionAssignment object with the list of actions the agent wants to execute.
The agent receives a copy of the GameState (with fog of war applied) and the forward model object for simulating the game. Depending on which game YAML-file was passed to 
Stratega, the forward model will behave accordingly.


Using C++
*********

.. role:: cpp(code)
   :language: c++

Do Nothing Agent
================

The simplest possible agent that can be created for Stratega is one that does nothing at all. Let's create this agent step by step (there's already a DoNothing agent implemented
in the framework, which you can consult if this tutorial is not clear).

First, let's create a folder for the header file. It's recommended that you implement your agent in a new folder. For instance, let's create the folder `include/Stratega/Agent/DoNothing`.

The header file of your agent should be placed in this folder. Let's create a new file here called `DoNohting.h`.

First, we need to include the headers. Because we need to extend the class Agent.h, we have to include that file:

.. code-block:: c++

    #pragma once
    #include <Stratega/Agent/Agent.h>

The agent needs to be in the same namespace as Stratega, which is SGA. Hence, the class needs to be enclosed in a namespace declaration with that name:


.. code-block:: c++

    namespace SGA
    {

    }


We'll call our agent's class ``DoNothing`` and we plan to override the method *computeAction*. Hence, our complete header file will look like this:

.. code-block:: c++

    #pragma once
    #include <Stratega/Agent/Agent.h>

    namespace SGA
    {
        class DoNothing : public Agent
        {
        public:
            using Agent::Agent;
            ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;
        };
    }

Next, it's time to give this function a body. You can either do it in the header file itself, or create a CPP file to do so. In this example, we'll do the later. Let's
create now another folder `DoNothing` in `src/Stratega/Agent/` and create a new CPP file in there called `DoNothing.cpp`. In this file, we'll add the following:

#. An include statement to include the header file for our DoNothing agent.
#. The SGA namespace block.
#. The signature of the *computeAction* function.

The code would look like this now:


.. code-block:: c++

    #include <Stratega/Agent/DoNothing/DoNothing.h>
    namespace SGA
    {
        ActionAssignment DoNothingAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
        {
        
        }
    }


Now, let's add the code require to do... nothing. For the moment, let's assume we're playing an RTS game. Given that RTS games don't have turns, we just return an empty action assignment. 
As simple as shown in the next snippet:

.. code-block:: c++

    #include <Stratega/Agent/DoNothing/DoNothing.h>

    namespace SGA
    {
        ActionAssignment DoNothingAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
        {
            return ActionAssignment();
        }
    }

And this is all. This code is compatible with the RTS and TBS types of games in Stratega, so you can use this agent for any of them straightaway.



Testing your agent
==================

In order to test your agent, you need to add it to Stratega's agent factory. Open `./src/Agent/AgentFactory.cpp <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/src/Agent/AgentFactory.cpp>`_ and:
#. add an include statement to the header file of your agent.
#. add a line with your agent name to the *getDefaultFactory* method.

For instance, for the DoNothing agent implemnted at './Stratega/include/Agent/DoNothing', you must:

#. Add all header (.h) files containing the agent to './Stratega/include/Agent/DoNothing/'
#. Open './Stratega/src/Agent/AgentFactory.cpp'
#. Add :cpp:`#include <Agent/DoNothing/DoNothing.h>` to the include-list
#. In the function *getDefaultFactory* add the line :cpp:`factory.registerAgent<DoNothing>("DoNothing");`

You should now be able to compile the framework with no errors, and to use the agent in any game of Stratega.

Finally, in order to *actually* use your agent in a game, you need to modify the list of agents in the configuration file of a game. For instance, let's open 'Stratega/resources/gameConfigurations/TBS/KillTheKing.yaml'.
At the top of the file, you'll see the section for the agents like the following:


.. code-block:: yaml

    Agents:
        - HumanAgent
        - CombatAgent

Substitute one of these agents for the one you've created, which has the "DoNothing" string as identifier. This is the same string as the one used in the *getDefaultFactory* fucntion. Hence,
it should now look like this:

.. code-block:: yaml

    Agents:
        - DoNothing
        - CombatAgent

You can now run your agent. Stratega has two different running modes: GUI and Arena. While the former allows you to run one game with visuals, the latter permits running multiple games with
graphics disabled. Each mode has its corresponding `main.cpp` file: 'src/gui/src/main.cpp' and 'src/arena/src/main.cpp'. You can execute these programmes indicating the path to the YAML file in the 
"-configPath" argument. Alternatively, you can also change the default YAML to be loaded in code, for instance for Arena:

.. code-block:: c++

    auto configPath = parser.getCmdOption<std::string>("-configPath", "../resources/gameConfigurations/TBS/KillTheKing.yaml");




Random Agent
============

The second simplest agent that can be implemented is the Random Agent. This agent returns actions at random at every call of the *computeAction* function. The process to
set up this agent is the same as for the DoNothing agent (and any other agent in the framework). Hence, the header file of the agent looks like this:


.. code-block:: c++

    #pragma once
    #include <Agent/Agent.h>

    namespace SGA
    {
        class MyAgent : public Agent
        {
        public:
            using Agent::Agent;
            ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;
        };
    }


What differs from the DoNothing agent is the content of the *computeAction* method. First, in order to be able to select which action to return, we need the collection of actions
available in the current game state. This can be retrieved from the forward model objecct, by using the following call:

.. code-block:: c++

    // Generate all available actions
	auto actions = forwardModel.generateActions(state, getPlayerID());


In this code, 'actions' is a vector of objects of type "Action" which contains all actions available in the current game state. Our next step is to choose a random element in this vector, 
which can be done by i) creating a uniform distribution from the vector; ii) picking one index from the distribution using the random number generator of the agent (provided by 
*getRNGEngine*); and 3) picking the action with the given index:

.. code-block:: c++

    // Uniformly sample a action
    boost::random::uniform_int_distribution<size_t> actionDist(0, actions.size() - 1);
    auto actionIndex = actionDist(getRNGEngine());
    auto action = actions.at(actionIndex);


Finally, we return an action assignment created from the action chosen at random. To do this, we use the static function *fromSingleAction* from the ActionAssignment class. This final
line completes the code for this function:



.. code-block:: c++
    :linenos:

    #include <boost/random.hpp>
    #include <Stratega/Agent/RandomAgent.h>

    namespace SGA
    {
        ActionAssignment RandomAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
        {
            // Generate all available actions
            auto actions = forwardModel.generateActions(state, getPlayerID());
            
            // Uniformly sample a action
            boost::random::uniform_int_distribution<size_t> actionDist(0, actions.size() - 1);
            auto actionIndex = actionDist(getRNGEngine());
            auto action = actions.at(actionIndex);
	
            // Return Action
            return ActionAssignment::fromSingleAction(action);
        }
    }


Note that this code is compatible for both RTS and TBS games.




Using Python
*************

.. role:: python(code)
   :language: python

.. note::
    Inside src/python/agents is a python script with a set of pre-implemented agents an the minimal code to run gui/arena using the included gameconfigurations of the python module.

Do Nothing Agent
============

The simplest possible agent that can be created for Stratega is one that does nothing at all. Let's create this agent step by step.

First, we need to import the python binary module produced from C++:

.. code-block:: python

    import stratega

We plan to override the virtual method *computeAction*. Hence, our class definition will look like this:

.. code-block:: python

    class DoNothingPythonAgent(stratega.Agent):
        def computeAction(self, state, forward_model, timer):
        

Now, let's add the code require to do... nothing. For the moment, let's assume we're playing an RTS game. Given that RTS games don't have turns, we just return an empty action assignment. 
As simple as shown in the next snippet:

.. code-block:: python

    class DoNothingPythonAgent(stratega.Agent):
        def computeAction(self, state, forward_model, timer):
                return stratega.ActionAssignment()

And this is all. This code is compatible with the RTS and TBS types of games in Stratega, so you can use this agent for any of them straightaway.


Random Agent
============

The second simplest agent that can be implemented is the Random Agent. This agent returns actions at random at every call of the *computeAction* function. The process to
set up this agent is the same as for the DoNothing agent (and any other agent in the framework). Hence, the class definition of the agent looks like this:


.. code-block:: python

    class RandomPythonAgent(stratega.Agent):
        def computeAction(self, state, forward_model, timer):


What differs from the DoNothing agent is the content of the *computeAction* method. First, in order to be able to select which action to return, we need the collection of actions
available in the current game state. This can be retrieved from the forward model objecct, by using the following call:

.. code-block:: python

    # Generate all available actions
	actions=forward_model.generate_actions(state, self.get_player_id())


In this code, 'actions' is a list of objects of type "Action" which contains all actions available in the current game state. Our next step is to choose a random element in this vector, 
which can be done by i) importing the random module to generate pseudo-random numbers; ii) picking one index from the random generator; and 3) picking the action with the given index:

.. code-block:: c++

    # Sample random action
    action_index=random.randint(0, actions.count()-1)
    action=actions.__getitem__(action_index)


Finally, we return an action assignment created from the action chosen at random. To do this, we use the static function *from_single_action* from the ActionAssignment class. This final
line completes the code for this function:



.. code-block:: python
    :linenos:
    import stratega

    class RandomPythonAgent(stratega.Agent):
        def computeAction(self, state, forward_model, timer):

            action_index=random.randint(0, actions.count()-1)
            action=actions.__getitem__(action_index)
            action_assignment=stratega.ActionAssignment.from_single_action(action)

            return action_assignment


Note that this code is compatible for both RTS and TBS games.