#############
Overview
#############

This page contains an overview of the various components contained in Stratega.
Stratega can be split roughly into 4 parts, i.e. the game-runner, rendering, agents and configuration.
The diagram below shows how the various components interact with each other.
Note that the game-runner is represented by the box containing the forward model and game state.

.. image:: ../../images/ArchitectureOverview.png
   :alt: Architecture Overview

The center of the framework is the game-runner, whose sole purpose is to run games.
Since Stratega supports both turn-based games and real-time games, the game-runner implements different behaviours based on the type of the game.
To actually run a game, the game-runner contains the current state of the game and a forward model.
The forward model is used by the game-runner to update the state of the game whenever it receives a new set of actions.
While the game state is the same for all game types, the forward model is implemented differently to suit the game type. 

Now, the game-runner may be able to run games.
However, it also needs access to someone who decides how to act in the game.
For this, the game-runner also gets access to a list of agents.
Whenever a player can act in the game, the game-runner will ask the corresponding agent to compute an action.
To do that, the agent gets access to a copy of the game state and the forward model.
Note that the game state sent to the agent will have fog of war applied to it.
In turn-based games, only one player can act simultaneously, so only one agent will be asked to compute an action.
In real-time games, all agents can act in parallel, so they run in separate threads to save time.

The game-runner can be executed in headless mode to evaluate games in quick succession.
But the game-runner also supports a way to visualize the game and play it manually.
For this, the game-runner has access to the game-renderer.
The game-renderer is continuously informed about the current game state, which is then displayed to the user.
By default, the GUI will be in spectator mode, but if the game-runner is missing an agent, the GUI will substitute the missing agent.

At last, the framework provides the ability to configure games using YAML files.
The configuration is used to generate everything that was discussed so far, this includes what is contained in the game state, the game rules used by the forward model, and you can even define default agents that should be used in the game.
The main benefit of the configuration file is that it allows researchers to test agents on a wide range of games.
You do not have to reimplement the agent for each game, implement the agent once and test it on all the games already configured for Stratega.
