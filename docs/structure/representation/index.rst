.. _game-state-representation:

.. role:: cpp(code)
   :language: c++

###########################
Game State Representation
###########################
This page describes several components that form the game state, as well as other aspects that are important parts of Stratega. The game state contains information about 
the elements that form the game at a given time. This is: players, entities, current board and general status of the game (current turn, if it's over, who won, etc).  
Visit the :ref:`cpp-reference <cpp-reference>` and the `GameState.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Representation/GameState.h>`_ 
file for more information. The rest of this page provides an introduction to important components of the game as defined in Stratega.

+++++++++++++++
Game Types
+++++++++++++++

As of today, Stratega allows creating turn-based and real-time strategy games. Independently of the type of game, most of the logic of the framework is common and shared within
the same classes. Therefore, Stratega uses the same objects to represent the information contained in turn-based and real-time games. The enum **GameType** is used to differentiate
the type of game that is being played in specific cases. 

.. code-block:: c++
    :caption: Enumeration for Game Type (`GameState.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Representation/GameState.h>`_)

	enum class GameType
	{
		TBS,
		RTS
	};

Here, we briefly describe our definition of the two types of games and their characteristics:

 - In **turn-based** games, only one player can play at a time. Player's play in a predefined order and this order cannot be changed. When a player can play, they can execute  actions until they run out of actions or end their turn by themselves. All actions are executed individually (i.e. one action after the other), not simultaneously. A turn is  over in turn-based games once *all* players have played once. After this, the next turn starts immediately.

 - In **real-time** games, all players can play at the same time. For tracking "time" in both game types, the game state contains a value called *ticks*. In real-time games,  a tick equates to 1/60 of a second. In each tick, players can simultaneously execute multiple actions, but it is impossible to execute multiple actions with the same unit or player. 

For turn-based games, a single *tick* is the equivalent of a turn.


.. _game-state-representation_gameInfo:
+++++++++++++++
Game Info
+++++++++++++++

Stratega provides functionality to writing bots for any of the games in the framework. As such, this information is divided into two categories: actual queries and effectors on the game state 
(actions to execute as a player, functions to know about the current state of the game, etc.), and information about the game as a whole (objectives, types of actions players can execute, types
of tiles that can possibly be found in the game, etc). The former type of information is *dynamic*, as it changes with the state of the game. The latter is *static*, as it applies to the game
as it has been designed. 

Static information about the game is given by the class GameInfo, in `GameInfo.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Representation/GameInfo.h>`_, 
which is accessible from any game state. GameInfo contains information about all types used by the game. This allows agent to access information about the game being played dynamically. 
This information refers to tiles, entities, actions, etc. that can be observed during the game, irrespective of the current game state. Concretely, this information includes:

#. *YAML Path*: Relative path to the game's YAML description file. This allows agents to parse and analyze the characteristics of the game being played.
#. *Entity parameters*: a list of all the parameters that different entities in the game can have (e.g. Heal Range, Movement Points, Attack Damage). Each parameter has a unique identifier and a representative string.
#. *Entity types*: list of all entity types existing in this game. Each entity type (e.g. Archer, Warrior, Worker) has a unique entity type identifier and a representative string.
#. *Action types*: list of all action types available in the game. Each action type (e.g. Attack, Heal, Move) has a unique type identifier and representative string.
#. *Tile types*: list of all types of tiles in the game. Each tile type has a unique identifier and a representative string (e.g. Forest, Water, Mountain, Plain). This includes a special type of tile  called "Fog of War", which is used for producing partial observability.
#. *Entity Groups*: super-set of *Entity Types*, also including groups of types that share common characterisitcs. These groups are used in Stratega to categorize types and apply effects on these categories rather than individual types. An example is shown in the code below:


.. code-block:: yaml
    :caption: Entity Groups (`BasicRTS.yaml <https://github.com/GAIGResearch/Stratega/blob/dev/resources/gameConfigurations/TBS/BasicTBS.yaml>`_)

    EntityGroups:
        Units: [Worker, Warrior, Archer, Catapult]
        Buildings: [City, Barracks, MilitaryAcademy, Foundry, Workshop, Storage]
        Attackable: [City, Barracks, MilitaryAcademy, Foundry, Workshop, Storage, Worker, Warrior, Archer, Catapult]


+++++++++++++++
Game State
+++++++++++++++
The Game State class `GameState.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Representation/GameState.h>`_ contains dynamic data information about a
particular game state. This includes information about the status of the game, like the current tick (or turn), if the game is over and (in that case) which player won, or who's turn
it is if the game is not over. It also contains information about the game type (RTS or TBS) and a pointer to GameInfo (described above)  for static information about the game.

Most importantly, the game state contains information about the following aspects, which are further described below in this page:

#. Information about the **players** of the game: their entities, paramters, number of players, etc.
#. Information about the current state of the **board**. 
#. Information about the **entities** present in the game.
#. **Technologies** that have been researched by each player.



+++++++++++++++
Player
+++++++++++++++

A player in Stratega takes control of one of the factions that is playing the game. Some factions (neutral) are not controlled by any player. Each player has a unique player ID and
a score. Players have certain *parameters*, which are values that affect the faction as a whole (i.e. amount of Gold the faction has). The definition of these parameters is included
in the GameInfo object, but their values can be easily accessed through the game state by either the player ID or parameter name. Each player also has information about the types of
actions that it can execute and a list of *durative* actions, which are executed during more than one tick or turn. 

For instance, in YAML, the definition of a player can be like the following snippet:

        
.. code-block:: yaml
    :caption: Entity Groups (BasicRTS.yaml) 
       
    Player:
        Actions: [Build, Research]
        CanSpawn: Buildings
        Parameters:
            Gold: 0
            Prod: 5



where a player is defined as having the ability of executing two actions (Build and Research), spawining entities of the entity group "Buildings" and has two parameters (Gold and Prod) 
with initial values of 0 and 5, respectively.

By default Stratega instantiates one player for each agent defined in the configuration. For instance:

.. code-block:: yaml
    :caption: Agent  (BasicRTS.yaml) 

    Agents:
    - OSLAAgent
    - CombatAgent

instantiates two players to play the given game: a one step look ahead (OSLA) agent and a rule based one (CombatAgent). These agents need to be defined in the method 
AgentFactory::getDefaultFactory(), in the file `AgentFactory.cpp <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/src/Agent/AgentFactory.cpp>`_.


..
    Here we should describe that you define a player-type in the confguration, which is then used to instantiate a list of players.
    Same as in Entities, a player-type is just used to instantiate a player in the game state.
    

    !!! Add example of player definition + maybe agents? !!!

    A player can have a score (ToDo we do not have something that actually increases the score, its difficult to define something like that)
    You can attach actions to players -> Link to the documentation Representation/GameLogic
    You can attach parameters to players, same as with entities

    This documentation should not go into much detail of the classes itself, since thats what the c++ documentation is for.
    But classes related to this part are:
        Stratega/Representation/Player <- A type describing a specific entity for example Warrior
        
    There is no PlayerType in Stratega right now, we should add that...
    The definition of actions and parameters are contained in the gameInfo itself
    Stratega/Representation/GameInfo
    contains std::shared_ptr<std::unordered_map<ParameterID, Parameter>> playerParameterTypes;
    and std::shared_ptr<std::unordered_map<int, ActionType>> actionTypes <---- !! This one contains the player actions, along with entity actions !!


    Player has: 
     - ID, score, canPlay, parameters, actions, attached actions.




+++++++++++++++
Board
+++++++++++++++

The board in Stratega is represented by a 2-dimensional array of tiles, which may be of different types (as defined in
the game YAML file) and hold one or more entities. The dimensions of this board are defined by the size of the game 
level provided in YAML format.

**Tiles** form the board and have a *tile type*, an <x,y> position, and two flags that indicate if i) the tile is
walkable; and ii) if it blocks link of sight between entities. Instantiations of Tile 
(`Tile.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Representation/Tile.h>`_) are dynamic, 
while their TileType (`TileType.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Representation/TileType.h>`_)
decribes static information, common for all types of the same type. Each tile type has a name, an associated character
(for YAML level instantiations), an ID and the values for walkable and line of sight blocking for this type.

A tile type is defined in YAML as follows:

.. code-block:: yaml
    :caption: Tile Type Mountain (BasicRTS.yaml) 

    Mountain:
        Sprite: ../../assets/Tiles/rock.png
        Symbol: M
        IsWalkable: false
        BlocksSight: true


A special type of tile is the *FogOfWar* tile. This tile is used when a particular position in the board is not visible to the
movin player. All entities hidden by fogOfWar are removed from the copy of the game state the player receives when is requested
to make a move in the game.

..
    Same as with players and entities, we define TileTypes which are used to generate Tiles
    TileTypes are not very complicated, we can copy the documentation from Tutorials/DefiningANewGame

    Tiles are stored in an rectangular grid which can be defined in the configuration.
    Again copy documentation from Tutorials/DefiningANewGame

    This documentation should not go into much detail of the classes itself, since thats what the c++ documentation is for.
    But classes related to this part are:
        Stratega/Representation/TileType
        Stratega/Representation/Tile
        Stratega/Representation/Grid2D <- A generic class that represents a 2d grid, Stratega uses Grid2D<Tile> to represent the board

+++++++++++++++
Entities
+++++++++++++++

The Game State holds all the entities that are present in a game, as defined in the YAML description file. This includes entities such 
as different units and buildings. Each entity is represented by a unique ID, an owner (one of the players, or neutral) and an entity type, 
and has a series of parameters that define their abilities and status. These parameters are also defined in the YAML file, and 
examples are Health, Attack Range, Attack Damage, Movement Points, etc.

Actions can be attached to entity types, so that Stratega automatically generates actions for entities of the corresponding type. 
An entity type (static information about the entities) defines the characteristics of the entity, such as name, YAML character 
symbol, cost, a list of parameters an entity holds, the actions that it can execute and the requirements for the entity to be 
present in the game (i.e. a certain technology  being researched).

The following YAML definition shows the entity type 'Archer' for a given game:

.. code-block:: yaml
    :caption: Entity Type 'Archer'

    Archer:
        Sprite: .../../assets/Entities/unit4.png
        Symbol: a
        LineOfSightRange: 10
        Parameters:
            Health: 100
            AttackRange: 5
            AttackDamage: 10
            MovementPoints: 2
        Actions: [Attack, Move]


Entity types can be seen as the 'templates' of a given entity, which will be given an ID, owner, position and initial value for its parameters when 
instantiated in the game and placed in the board.

Entities can be retrieved from the `GameState.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Representation/GameState.h>`_ 
by its position, entity ID or player ID (which returns a collection of all entities owned by a player). 

For more information regarding entities, check the following classes and the :ref:`cpp <cpp-reference>` documentation:
 - `EntityType.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Representation/EntityType.h>`_
 - `Entity.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Representation/Entity.h>`_

..
    Here we should describe how we represent Units, Buildings, etc
    The core idea is that entities have a position and nothing else.
    To make a entity a building we can then add parameters to that building (For example gold)
    So essentially make sure that users understand that everything is represented by an entity.
    We could also mention that entities are owned by a specific player and that entities can be neutral.

    We should also mention that you can attach actions to entities, also mention actions are described in Representation/GameLogic
    When attaching actions you essentially tell Stratega to generate actions whenever it finds an entity of the corresponding entity type

    !!! Add an example of an YAML-Definition for entitity-type and explain the difference between entities and entity-types !!!

    This documentation should not go into much detail of the classes itself, since thats what the c++ documentation is for.
    But classes related to this part are:
        Stratega/Representation/EntityType <- A type describing a specific entity for example warrior
        Stratega/Representation/Entity <- The entity itself, meaning it is placed on the board
        Stratega/Representation/Parameter <- A parameter stored in the entity for example gold

    Entity Type has:
     - id (type):
     - name
     - parameter

    Entity has:
     - typeID, id, owner, position, lineOfSight, path, movementSpeed, collisionRadius. 
     - isNeutral.



+++++++++++++++++++++++++
Research and Technologies
+++++++++++++++++++++++++

Stratega incorporates the concept of research trees for strategy games. The fundamental atomic element is a 
Technology (implemented in the struct "TechnologyTreeNode"), which is organized in one or more trees in a game
(it is possible to define multiple technologies trees in a game). 
Each technology incudes the following elements:

#. Name of the technology
#. Description of the technology
#. Technologies that need to be researched before this one becomes available.
#. List of costs that must be paid to research this technologies.
#. Time (ticks or turns) required to investigate this technology.

An example of a technology as defined in YAML is as follows:

.. code-block:: yaml
    :caption: Technology 'Pottery' in BasicTBS game.

    Pottery:
        Description: Allows to construct a Storage.
        Requirements: [Mining]
        Cost:
            Prod: 10
        Time: 2

In this example, the technology "Potery" has a string description that defines what can this be used for. It has a requirement, "Mining", which
is another technology that must be researched before this one becomes available. It costs 10 units of the resource "Prod" and takes 2 turns to
be completed.

Technologies can be researched through actions, and the code includes functions to inspect the technology trees 
(in `GameInfo.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Representation/GameInfo.h>`_) and to check if a 
technology can be researched or has been researched already 
(in `GameState.h <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Representation/GameState.h>`_).


