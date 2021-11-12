.. _first_steps_python:

.. role:: python(code)
   :language: python

####################
First steps (Python)
####################

This pages shows how to run Stratega in the different modes its offers and shows how to test your own Python agents. Stratega uses pybind11 which provides an elegant way to wrap C++ code for Python.

First, let's be sure that the option "STRATEGA_BUILD_BINDINGS" in the Stratega CMakeList.txt is enabled:

.. code-block:: cmake

    option(ENABLE_STRATEGA_BINDINGS "Enable to build the bindings to other languages." ON)

This option will generate a pyd file that contains a Python module, to be called by other Python code. The module can be found in the folder src/python once the build is completed.

++++++++++++++++++++++
Installing from pip
++++++++++++++++++++++

Instead of building the project from the source project, you can run the following command on the console using pip:

.. code-block:: cmake

    pip install stratega


++++++++++++++++++++++
Stratega running modes
++++++++++++++++++++++

Stratega has two different running modes: GUI and Arena. While the former allows you to run one game with visuals, the latter permits running multiple games with graphics disabled. 

GUI mode
+++++++++

In this section we will learn how to:

#. Load the game configuration.
#. Create a game runner.
#. Generate a list of agents from the configuration.
#. Run the GUI with a list of agents.

First, we need to import the python binary module produced from C++:

.. code-block:: python

    import stratega

Next, it's time to read the Yaml configuration and store it in a varible. The following method *load_config* receives the file path of the yaml and generates a `GameConfig.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Configuration/GameConfig.h>`_ object.

.. code-block:: python

    config = stratega.load_config("resources/gameConfigurations/TBS/KillTheKing.yaml")

.. note::
    When you install the python module, it also installs the main resource folder with all the default game configurations and sprites. You can load them using any path that begins with: "resources/gameConfigurations/..."

Once we have the configuration stored, we need to create the GameRunner by passing as argument the previous stored configuration.

.. code-block:: python

    runner = stratega.create_runner(config)

Now, lets generate the list of agents from the configuration:

.. code-block:: python

    config_agents = stratega.generate_agents(config)

The last step is to call the *play* method of the the GameRunner and  pass a list of agents with a seed to intialize them and a window resolution.

.. code-block:: python

    resolution=stratega.Vector2i(1920,1080)
    runner.play(config_agents, resolution, 0)

The complete GUI call would looks as follows:

.. code-block:: python

    import stratega

    if __name__ == "__main__": 
        
        config = stratega.load_config("resources/gameConfigurations/TBS/KillTheKing.yaml")
        runner = stratega.create_runner(config)    
        
        config_agents = stratega.generate_agents(config)

        resolution=stratega.Vector2i(1920,1080)
        runner.play(config_agents, resolution, 0)

To execute Stratega with your own python agents, check the next section below: Testing your Python agent

.. note::
    Inside src/python/agents is a python script with a set of pre-implemented agents an the minimal code to run gui/arena using the included gameconfigurations of the python module

Arena mode
++++++++++

In this section we will learn how to:

#. Load the game configuration.
#. Define the Arena parameters.
#. Create the Arena.
#. Load a set of Maps.
#. Run the Arena.

Lets start importing our python module and loading the game configuration, they are exactly the same lines we used when running in GUI mode.

.. code-block:: python

    import stratega

    if __name__ == "__main__": 
        
        config = stratega.load_config("resources/gameConfigurations/TBS/KillTheKing.yaml")

.. note::
    When you install the python module, it also installs the main resource folder with all the default game configurations and sprites. You can load them using any path that begins with: "resources/gameConfigurations/..."

The Arena will need some parameters and it is time to define them. If you want to check in details what it means each parameter, check the :ref:`Arena <_arena>`.

.. code-block:: python
    
    number_of_games=1
    player_count=2
    log_path="./sgaLog.yaml"
    maps_path=""
    seed=0

Next step is to set the default logger with a path. You can check the log once the Arena finished computing all the battles.

.. code-block:: python

    stratega.set_default_logger(log_path)

We are almost done, now is time to create the arena using the previous game configutation.

.. code-block:: python

    arena=stratega.create_arena(config) 

The last thing is to let know the Arena if it should execute the games in a set of maps or with the default map from the configuration.

.. code-block:: python 

    if not maps_path:
        arena.run_games(player_count,seed, number_of_games,1)
    else:
        config.level_definitions= stratega.load_levels_from_yaml(maps_path, config)
        map_number=len(config.level_definitions)
        arena.run_games(player_count, seed, number_of_games,map_number)

The complete arena call would looks as follows:

.. code-block:: python

    import stratega

    if __name__ == "__main__": 
        
        config = stratega.load_config("resources/gameConfigurations/TBS/KillTheKing.yaml")

        number_of_games=1
        player_count=2
        log_path="./sgaLog.yaml"
        maps_path=""
        seed=0

        stratega.set_default_logger(log_path)
        arena=stratega.create_arena(config)

        if not maps_path:
            arena.run_games(player_count,seed, number_of_games,1)
        else:
            config.level_definitions= stratega.load_levels_from_yaml(maps_path, config)
            map_number=len(config.level_definitions)
            arena.run_games(player_count, seed, number_of_games,map_number)

.. note::
    Inside src/python/agents is a python script with a set of pre-implemented agents an the minimal code to run gui/arena using the included gameconfigurations of the python module


++++++++++++++++++++++++++
Testing your Python agent
++++++++++++++++++++++++++


We will use this Random Agent that we implemented in the previous tutorial, it would be advisable to take a look at the following tutorials to understand how to implement Agents in python

#. :ref:`Implementing Simple AI Agents (Python) <_implement_python_agent>`
#. :ref:`Implementing One Step Look Ahead (Python) <_implement_python_osla>`

.. code-block:: python

    class RandomPythonAgent(stratega.Agent):
        def computeAction(self, state, forward_model, time_budget_ms):     
            actions=forward_model.generate_actions(state, self.get_player_id())
            action=actions.__getitem__(random.randint(0, actions.count()-1))
            action_assignment=stratega.ActionAssignment.from_single_action(action)

            return action_assignment

To use your own agents we need to create a agent list that can hold stratega in-built agents and python agents.  If you want to use an in-built Stratega agent, you can define it as a string.

Lets check the following example that creates a list of two agents, the first one is the in-built "DoNothingAgent" and the second one is our "RandomPythonAgent";

.. code-block:: python

    agent_list=["DoNothingAgent", RandomPythonAgent()]

You can find all the in-built agents in `./src/Agent/AgentFactory.cpp <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/src/Agent/AgentFactory.cpp>`_.

GUI mode
++++++++

The method *play* can be overloaded with a list of agents. The agents are defined the same way we did previously, using strings to define the in-built agents and the constructor of our python agent.

.. code-block:: python

    resolution=stratega.Vector2i(1920,1080)
    runner.play(["HumanAgent", RandomPythonAgent()], resolution, 0)


The complete GUI call would looks as follows:

.. code-block:: python

    import stratega

    if __name__ == "__main__": 
        
        config = stratega.load_config("resources/gameConfigurations/TBS/KillTheKing.yaml")
        runner = stratega.create_runner(config)    
        
        config_agents = stratega.generate_agents(config)

        resolution=stratega.Vector2i(1920,1080)
        runner.play(["HumanAgent", RandomPythonAgent()], resolution, 0)

Arena mode
++++++++++

As the previous example, the main code to read the configuration and create the Arena keeps being the same. We only need to change the *run_games* call and add the list of agents we want to play with.

.. code-block:: python

    #run with default map
    arena.run_games(player_count, seed, number_of_games,1,["CombatAgent", RandomPythonAgent()])

.. code-block:: python

    #run arena with a set of maps
    arena.run_games(player_count, seed, number_of_games,map_number, ["CombatAgent", RandomPythonAgent()])


The complete arena call would looks as follows:

.. code-block:: python

    import stratega

    if __name__ == "__main__": 
        
        config = stratega.load_config("resources/gameConfigurations/TBS/KillTheKing.yaml")

        number_of_games=1
        player_count=2
        log_path="./sgaLog3.yaml"
        maps_path=""
        seed=0

        stratega.set_default_logger(log_path)
        arena=stratega.create_arena(config)

        if not maps_path:
            # run with python agents
            arena.run_games(player_count, seed, number_of_games,1,["CombatAgent", RandomPythonAgent()])
        else:
            config.level_definitions= stratega.load_levels_from_yaml(maps_path, config)
            map_number=len(config.level_definitions)
            # run with python agents
            arena.run_games(player_count, seed, number_of_games,map_number, ["CombatAgent", RandomPythonAgent()])

