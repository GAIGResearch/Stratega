.. role:: cpp(code)
   :language: c++

#############################
Agents and Game States
#############################

AI Agents have access to the current game state in order to be able to determine what is the best next action to execute in the game. This page shows a few examples about how to
access interesting information in the game state that can help you implement agents for this framework. Stratega supplies information in two different modalities:

#. Static information: this is information about the game as it is defined. This information doesn't change from state to state - i.e. it's information about the game rules.
#. Dynamic information: includes all the elements of the game state, that changes from one instantiation of the game to another. Examples are the presence of units, buildings, player parameter values and technologies researched. 

Given that Stratega supports multiple games (defined via YAML files), the *static* information is useful for creating agents that can include domain knowledge for multiple games, or to train
agents that behave differently depending on the characteristics of the game being played. The *dynamic* information is, however, useful for decision making in a state to state basis and, with
the functionality of the forward model, permits the implementation of model-based and/or sampling agents.

This page shows examples of how agents can access this information. For a more lengthy description of the elements of the game state and its representation, see 
:ref:`Game State Representation <game-state-representation>`.


++++++++++++++++
Game Information
++++++++++++++++

First of all, the YAML description file of the game being played is accessible to the agents at all times. You can retrieve the filename and parse the YAML file yourself. For instance, 
like this:

.. code-block:: c++
    :linenos:

    std::string yamlPath = state.gameInfo->yamlPath;
    std::ifstream infile(yamlPath);
    std::string line;
    while (std::getline(infile, line))
    {
        // Each iteration is a 'line' in the YAML file.
    }


Note that 'state' in line 1 is the object of type 'GameState' which is (for instance) received in the function *computeAgent*. The property gameInfo in the game state is in charge of
providing static information about the game. See section :ref:`Game Info <game-state-representation_gameInfo>` for more details.

GameInfo's API provides information about the types of objects that can be found in the game. In particular, you can find information about possible types of tiles that form the board, 
entities and actions. These types can be accessed directly in their data structure (3 unordered maps, respectively, indexed by unique type ID) or by supplying a specific ID for the type.
The following code snippet shows how to access these maps and prints the name of the different elements in the game:

.. code-block:: c++
    :linenos:

    auto entityTypes = state.gameInfo->entityTypes;
    for (const auto& [id, eType] : *entityTypes) 
        std::cout << "[Entity] id: " << id << ", name: " << eType.name << std::endl;

    auto tileTypes = state.gameInfo->tileTypes;
    for (const auto& [id, tType] : *tileTypes) 
        std::cout << "[Tile] id: " <<  id << ", name: " << tType.name << std::endl;

    auto actionTypes = state.gameInfo->actionTypes;
    for (const auto& [id, aType] : *actionTypes) 
        std::cout << "[Action] id: " << id << ", name: " << aType.name << std::endl;


This code produces the following output for the game 'KillTheKing': 

.. code-block:: c++

    [Entity] id: 0, name: Archer
    [Entity] id: 1, name: Healer
    [Entity] id: 2, name: King
    [Entity] id: 3, name: Warrior
    [Tile] id: -1, name: FogOfWar
    [Tile] id: 0, name: Forest
    [Tile] id: 1, name: Mountain
    [Tile] id: 2, name: Plain
    [Tile] id: 3, name: Water
    [Action] id: 0, name: Attack
    [Action] id: 1, name: Heal
    [Action] id: 2, name: Move

All these *type* classes have information about the entities, tiles and actions they represent, such as:

#. `EntityType.h <https://github.com/GAIGResearch/Stratega/blob/dev/Stratega/include/Stratega/Representation/TileType.h>`_: Spawn/Build requirements, spawn costs, line of sights and actions that can be executed by this entity.
#. `TileType.h <https://github.com/GAIGResearch/Stratega/blob/dev/Stratega/include/Stratega/Representation/TileType.h>`_: Properties of the tile, such as if it's walkable or blocks the line of sight.
#. `ActionType.h <https://github.com/GAIGResearch/Stratega/blob/dev/Stratega/include/Stratega/Representation/ActionType.h>`_: Action source (player or entity), preconditions for the action to take place, list of effects and action targets.