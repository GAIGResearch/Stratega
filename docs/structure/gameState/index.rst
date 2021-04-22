#############
GameSate
#############

The Game-state object serves as a data container without any logic. 
It offers access to the currentboard, its tiles, a list of players, and their units as well as definitions of action and entity types. Dueto the strict separation of data and game logic, players receiving a game-state object can freely copyor modify it.

The board is represented as ann×m-grid of tiles. A tile can be occupied by a unit and will thereforebe blocked for other units. 
Tile types and board layouts are defined via the configuration file.  
The player’s observation of the board is limited by the line of sight of the player’sentities. 
Each unit has a perception radius which removes the fog of war and shows the real contentof a tile.  
Tiles outside this perception radius will be replaced with a default tile.  
Alternatively,games can be configured to track the agents knowledge of a tiles last known state, which will bereturned in case the none of the agent’s units is close enough to observe a tile’s current state.

An entity is a player controllable character or building which is able to perform one or multipleactions per turn. 
Each player controls all its entities at once, meaning that during a single "turn" theagent may return multiple actions. 
In terms of real-time strategy games, we consider a turn to be thetime-span until an agent returns a complex action for all its entities. In contrast to the turn-basedgame-mode, agents are acting simultaneously. 
This can cause actions to be not applicable anymore.Such actions will be ignored by the forward model.