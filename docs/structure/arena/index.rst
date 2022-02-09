.. _arena:

#############
Arena
#############

After coding new agents and environments you maybe would wonder how to test them simply without rendering the game state and manually executing each game.

.. image:: ../../images/arenaArguments.png
   :alt: Arena Structure

The Arena provides an easy way to test the performance between different Agents in different environments. 
It lets play a set of Agents versus each other while using the logging system it generates automatically a file with all the information about the game.

Arena flow
----------
An arena competition is a set of Games between two or more agents. 
Each Game consists of a certain number of battles, the number of battles depends on the number of possible permutations between the number of agents.
Each battle uses the same seed to generate the Gamestate.
In each permutation the agents order are swapped, this gives the possibility to let play the agents from all the different start positions. 

    **Permutation example:**

    Given the following list of three agents.

    Agents list:
        - MCTSAgent
        - RHEAAgent
        - RandomAgent

    The Arena will generate a list of combinations and run each combination using the same seed for each battle.

    Possible battles in a Game:
        - MCTSAgent vs RHEAAgent vs RandomAgent
        - MCTSAgent vs RandomAgent vs RHEAAgent
        - RHEAAgent vs MCTSAgent vs RandomAgent
        - RHEAAgent vs RandomAgent vs MCTSAgent
        - RandomAgent vs RHEAAgent vs MCTSAgent
        - RandomAgent vs MCTSAgent vs RHEAAgent

The Arena can be configured to play a certain number of Games resulting in faster testing.
Each game will increment the seed for obtaining different results while using the same battle configuration.
It is also possible to run a set of games on different maps.


Arena Arguments
----------------
The arena accepts multiple arguments to configure how the competition will take place.
Some of these are optional and Stratega already included a default value.

Arguments:

- **seed:** Used to initialize the random engine.
- **playerCount:** Defines the number of agents that will play the games.
- **logPath:** Path of the log file that Stratega will generate.
- **gamesNumber:** Allow the user to choose the number of games that agents will play.
- **configPath:** Path of the game configuration yaml.
- **mapsPath:** Allow the user to play all game combinations in a set of maps.

+------------+------------+------------------+
| Argument   | Optional   | Default value    |
+============+============+==================+
| seed       |    yes     |        0         |
+------------+------------+------------------+
| playerCount|    yes     |        2         |
+------------+------------+------------------+
| gamesNumber|    yes     |        2         |
+------------+------------+------------------+
| configPath |    no      | KillTheKing.yaml |
+------------+------------+------------------+
| mapsPath   |   yes      |        x         |
+------------+------------+------------------+
| logPath    |   yes      | ./sgaLog.yaml    |
+------------+------------+------------------+

Computation budget time
-----------------------
        
The arena can limit the computation and initialization time of the agents, these settings can be added to any game configuration.
If the agents exceed the budged time they will receive and warning, if the agent exceeds the number of warnings or the disqualification time will lose the battle.

.. code-block:: yaml
    :caption: Note that times are defined in milliseconds.

    GameRunner:
        AgentInitializationTime:
            Enabled: true
            BudgetTimeMs: 50
            DisqualificationTimeMs: 70
        AgentComputationTime:
            Enabled: true
            BudgetTimeMs: 50
            DisqualificationTimeMs: 70
            MaxNumberWarnings: 5


Single map
----------------
.. image:: ../../images/arenaSingleMap.png
   :alt: Single map example


++++++++++++++++++++++++++++++++++++++++++
Generated log example using one single map
++++++++++++++++++++++++++++++++++++++++++
This example is the result of a competition between two agents(OSLAAgent and RandomAgent). In this competition they only played one game in one single map.

.. code-block:: yaml

    Game 0:
        Map: 0
        Seed: 0
        Battle0:
            PlayerAssignment: [OSLAAgent, RandomAgent]
            ActivePlayer: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            ActionCount: [93, 95, 86, 80, 82, 72, 68, 59, 51, 47, 47, 46, 47, 47, 38, 35, 35, 26, 25, 23, 14, 12, 12, 1, 102, 96, 97, 88, 82, 81, 75, 71, 57, 49, 45, 44, 43, 40, 43, 36, 32, 30, 23, 22, 20, 10, 4, 6, 1, 101, 95, 97, 87, 84, 73, 63, 60, 60, 61, 62, 62, 51, 48, 47]
            WinnerID: 1
            Turns: 3
        Battle1:
            PlayerAssignment: [RandomAgent, OSLAAgent]
            ActivePlayer: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            ActionCount: [93, 95, 86, 80, 82, 72, 68, 59, 51, 47, 47, 46, 47, 47, 38, 35, 35, 26, 25, 23, 14, 12, 12, 1, 102, 96, 97, 88, 82, 81, 75, 71, 57, 49, 45, 44, 43, 40, 43, 36, 32, 30, 23, 22, 20, 10, 4, 6, 1, 101, 95, 97, 87, 84, 73, 63, 60, 60, 61, 62, 62, 51, 48, 47]
            WinnerID: 1
            Turns: 3


Multiple maps
--------------------
.. image:: ../../images/arenaMultiMap.png
    :alt: Multi map example

If you want to run all the generated combinations in a set of different maps, you can use the argument **-mapsPath** to define the path of yaml file with all the maps.

Example of maps file:

.. code-block:: yaml

    Maps:
        Map1: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  a1 .  .  .  .  .  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  a1 k1 h1 .  .  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  w1 w1 .  .  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  M  M  .  .  .  .  .  M  M  M
            M  .  .  .  .  .  .  .  .  .  W  W  W  W  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  W  W  W  W  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  M  M  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  .  W  W  W  W  W
            M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  a0 .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  a0 k0 h0 .  .  .  .  .  M
            M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  w0 w0 .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M

        Map2: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  a1 .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  a1 k1 h1 .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  w1 w1 .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M  M  .  .  .  .  .  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W
            M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  a0 .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  a0 k0 h0 .  .  .  .  .  M
            M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  w0 w0 .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M

        Map3: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  a1 .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  a1 k1 h1 .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  w1 w1 .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M  M  .  .  .  .  .  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  M  M  M  M  M  M  M  M  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  M  M  .  .  .  M  M  M  M  M  M  M  M  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  M  M  M  M  M  M  M  M  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  M  M  M  M  M  M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W
            M  M  M  .  .  .  M  M  M  M  M  M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W
            M  .  .  .  .  .  M  M  M  M  M  M  M  M  .  .  .  .  .  .  .  .  .  .  a0 .  .  .  .  .  .  M
            M  .  .  .  .  .  M  M  M  M  M  M  M  M  .  .  .  .  .  .  .  .  .  a0 k0 h0 .  .  .  .  .  M
            M  M  M  .  .  .  M  M  M  M  M  M  M  M  .  .  .  .  .  .  .  .  .  .  w0 w0 .  .  .  .  .  M
            M  .  .  .  .  .  M  M  M  M  M  M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
    

++++++++++++++++++++++++++++++++++++++++++
Generated log example using multiple maps
++++++++++++++++++++++++++++++++++++++++++
This example is the result of a competition between two agents(OSLAAgent and RandomAgent). In this competition they played only one game on three different maps.

.. code-block:: yaml

    Game 0:
        Map: 0
        Seed: 0
        Battle0:
            PlayerAssignment: [OSLAAgent, OSLAAgent]
            ActivePlayer: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            ActionCount: [93, 95, 86, 80, 82, 72, 68, 59, 51, 47, 47, 46, 47, 47, 38, 35, 35, 26, 25, 23, 14, 12, 12, 1, 102, 96, 97, 88, 82, 81, 75, 71, 57, 49, 45, 44, 43, 40, 43, 36, 32, 30, 23, 22, 20, 10, 4, 6, 1, 101, 95, 97, 87, 84, 73, 63, 60, 60, 61, 62, 62, 51, 48, 47]
            WinnerID: 1
            Turns: 3
        Battle1:
            PlayerAssignment: [OSLAAgent, OSLAAgent]
            ActivePlayer: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            ActionCount: [93, 95, 86, 80, 82, 72, 68, 59, 51, 47, 47, 46, 47, 47, 38, 35, 35, 26, 25, 23, 14, 12, 12, 1, 102, 96, 97, 88, 82, 81, 75, 71, 57, 49, 45, 44, 43, 40, 43, 36, 32, 30, 23, 22, 20, 10, 4, 6, 1, 101, 95, 97, 87, 84, 73, 63, 60, 60, 61, 62, 62, 51, 48, 47]
            WinnerID: 1
            Turns: 3
    Game 1:
        Map: 1
        Seed: 1
        Battle0:
            PlayerAssignment: [OSLAAgent, OSLAAgent]
            ActivePlayer: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            ActionCount: [93, 95, 86, 80, 82, 72, 68, 59, 51, 47, 47, 46, 47, 47, 38, 35, 35, 26, 25, 23, 14, 12, 12, 1, 102, 96, 97, 88, 82, 81, 75, 71, 57, 49, 45, 44, 43, 40, 43, 36, 32, 30, 23, 22, 20, 10, 4, 6, 1, 101, 95, 97, 87, 84, 73, 63, 60, 60, 61, 62, 62, 51, 48, 47]
            WinnerID: 1
            Turns: 3
        Battle1:
            PlayerAssignment: [OSLAAgent, OSLAAgent]
            ActivePlayer: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            ActionCount: [93, 95, 86, 80, 82, 72, 68, 59, 51, 47, 47, 46, 47, 47, 38, 35, 35, 26, 25, 23, 14, 12, 12, 1, 102, 96, 97, 88, 82, 81, 75, 71, 57, 49, 45, 44, 43, 40, 43, 36, 32, 30, 23, 22, 20, 10, 4, 6, 1, 101, 95, 97, 87, 84, 73, 63, 60, 60, 61, 62, 62, 51, 48, 47]
            WinnerID: 1
            Turns: 3
    Game 2:
        Map: 2
        Seed: 2
        Battle0:
            PlayerAssignment: [OSLAAgent, OSLAAgent]
            ActivePlayer: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            ActionCount: [93, 95, 86, 80, 82, 72, 68, 59, 51, 47, 47, 46, 47, 47, 38, 35, 35, 26, 25, 23, 14, 12, 12, 1, 102, 96, 97, 88, 82, 81, 75, 71, 57, 49, 45, 44, 43, 40, 43, 36, 32, 30, 23, 22, 20, 10, 4, 6, 1, 101, 95, 97, 87, 84, 73, 63, 60, 60, 61, 62, 62, 51, 48, 47]
            WinnerID: 1
            Turns: 3
        Battle1:
            PlayerAssignment: [OSLAAgent, OSLAAgent]
            ActivePlayer: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            ActionCount: [93, 95, 86, 80, 82, 72, 68, 59, 51, 47, 47, 46, 47, 47, 38, 35, 35, 26, 25, 23, 14, 12, 12, 1, 102, 96, 97, 88, 82, 81, 75, 71, 57, 49, 45, 44, 43, 40, 43, 36, 32, 30, 23, 22, 20, 10, 4, 6, 1, 101, 95, 97, 87, 84, 73, 63, 60, 60, 61, 62, 62, 51, 48, 47]
            WinnerID: 1
            Turns: 3
