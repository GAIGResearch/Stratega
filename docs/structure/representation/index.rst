.. role:: cpp(code)
   :language: c++

###########################
Game State Representation
###########################
This page describes several components that form the game state, as well as other aspects that are important parts of Stratega.

+++++++++++++++
Game Types
+++++++++++++++
Stratega uses the same object to represent the information contained in turn-based and real-time games. Here, we briefly describe our definition of the two types of games and their characteristics.

.. code-block:: c++
	enum class GameType
	{
		TBS,
		RTS
	};

In **turn-based** games, only one player can play at a time.
Player's play in a predefined order and this order cannot be changed.
When a player can play, they can execute actions until they run out of actions or end their turn by themselves.
All actions are executed individually (i.e. one action after the other), not simultaneously.
A turn is over in turn-based games once all players have played once. After this, the next turn starts immediately.

In **real-time** games, all players can play at the same time. For tracking "time" in both game types, the game state contains a value called *ticks*. In real-time games, a tick equates to 1/60 of a second. In each tick, players can simultaneously execute multiple actions, but it is impossible to execute multiple actions with the same unit or player. 

For turn-based games, a single *tick* is the equivalent of a turn.

+++++++++++++++
Entities
+++++++++++++++
..
    Here we should describe how we represent Units, Buildings, etc
    The core idea is that entities have a position and nothing else.
    To make a entity an building we can then add parameters to that building (For example gold)
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


+++++++++++++++
Player
+++++++++++++++
..
    Here we should describe that you define a player-type in the confguration, which is then used to instantiate a list of players.
    Same as in Entities, a player-type is just used to instantiate a player in the game state.
    By default Stratega instantiates one player for each agent defined in the configuration.

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


+++++++++++++++
Board
+++++++++++++++
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
        
++++++++++++
FogOfWar
++++++++++++
..
    Im unsure where to put this, but we should explain how fog of war works.
    Atleast that it replaces tiles with an default FogOfWar tile, which can be found in the game state.
    All entities hidden by fogOfWar are removed from the game state.
