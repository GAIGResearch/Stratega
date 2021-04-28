################################
Defining a new game
################################

In this tutorial we are going to define a game from scratch using the yaml configurations.
The objective is to create a game of combat type, in which a humar player fights a random agent.

.. code-block:: yaml

GameConfig:
    Type: TBS
    ActionTimer: 10s
    RoundLimit: 100

Agents:
    - RandomAgent
    - HumanAgent

Lets starts with the type of game and the type of agents

.. code-block:: yaml

Player:
    Parameters:
        SpawnPoints: 5

Next, the parameters of the players.

.. code-block:: yaml

Tiles:
    Plain:
        Sprite: ../../GUI/Assets/Tiles/plain.png
        Symbol: .
        IsWalkable: true
        DefaultTile: true
    Water:
        Sprite: ../../GUI/Assets/Tiles/water.png
        Symbol: W
        IsWalkable: false
    Mountain:
        Sprite: ../../GUI/Assets/Tiles/rock.png
        Symbol: M
        IsWalkable: false
     
The next step is defininf the tile our map will use. In this case a plain, water and mountain tiles.
We want the water and mountain tiles to not be walkable, then change the IsWalkable bool to false. Dont forget to write the symbol because it will be used later to write by hand the map.



.. code-block:: yaml

Entities:
    City:
        Sprite: ../../GUI/Assets/Entities/castle.png
        Symbol: c
        LineOfSightRange: 5
        CanSpawn: Fighter
        Actions: [Spawn]
        Parameters:
            Health: 100

    Fighter:
        Sprite: ../../GUI/Assets/Entities/unit2.png
        Symbol: f
        LineOfSightRange: 6
        Actions: [Move, Attack]
        Parameters:
            Health: 100
        Cost:
            SpawnPoints: 3

Is time to define the entities, every entity needs a sprite path, symbol and a LineOfSightRange value (used to generate the fog).
For this game we only need two simple entities, the city will be an entity that the player should protect and the fighter used to attack the enemy.

.. code-block:: yaml
     
Board:
    GenerationType: Manual
    Layout: |-
        M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
        M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  M
        M  M  .  .  .  .  .  .  c1 .  .  .  .  .  .  M  M
        M  M  .  .  .  .  .  .  .  .  .  .  M  M  .  M  M
        M  M  .  .  .  .  .  .  W  .  .  .  M  M  .  M  M
        M  M  .  .  .  .  .  .  W  .  .  .  .  .  .  .  M
        M  M  .  .  .  .  .  W  W  W  .  .  .  .  .  .  M
        M  .  .  .  .  .  W  W  W  W  W  .  .  .  .  .  M
        M  .  .  .  .  .  .  W  W  W  .  .  .  .  .  .  M
        M  .  .  .  .  .  .  .  W  .  .  .  .  .  .  .  M
        M  M  .  .  M  M  .  .  W  .  .  .  .  .  .  M  M
        M  M  .  .  M  M  .  .  .  .  .  .  .  .  .  M  M
        M  M  .  .  .  .  .  .  c0 .  .  .  .  .  .  M  M
        M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  M
        M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M


Once we have defined the entities and tiles, we can define the map using the symbols of both of them.
Note that the number attached to a entity symbolaze the player owner of the entity. (If you want a neutral entity, dont add any number)

.. code-block:: yaml

Actions:
    Spawn:
        Type: EntityAction
        Cooldown: 1
        Targets:
            Target:
                Type: EntityType
                ValidTargets: Fighter
                Conditions:
                    - "CanAfford(Source.Player, Target)"
                    - "CanSpawn(Source, Target)"
        Effects:
            - "SpawnRandom(Source, Target)"
            - "PayCost(Source.Player, Target)"

    # Attack Actions
    Attack:
        Type: EntityAction
        Cooldown: 1
        Targets:
            Target:
                Type: Entity
                ValidTargets: [Fighter, City]
                Conditions:
                    - "InRange(Source, Target, 1)"
        Effects:
            - "Attack(Target.Health, 25)"


    # Move Actions
    Move:
        Type: EntityAction
        Cooldown: 1
        Targets:
            Target:
                Type: Position
                Shape: Circle
                Size: 3
                Conditions:
                    - "IsWalkable(Target)"
        Effects:
            - "Move(Source, Target)"

We are almost done, we need to add the actions. We only need three actions: Move, Attack and Spawn. An action defines the inteaction between entities and player.

.. code-block:: yaml
                   
ForwardModel:
    LoseConditions: #If true: Player -> cant play
        NoHasCity:
        - "NoHasEntity(Source, City)"

    Trigger:
    - OnTick:
        ValidTargets: City
        Conditions:
            - "IsPlayerEntity(Source)"
        Effects:
            - "ModifyResource(Source.Player.SpawnPoints, 1)"


The last part is defining the lose/win conditions and the triggers. In this game we only have one simple rule, the player that lose his city will lose.
There is only one trigger to increment the number of spawn points of the player.

.. code-block:: yaml

GameConfig:
    Type: TBS
    ActionTimer: 10s
    RoundLimit: 100

Agents:
    - RandomAgent
    - HumanAgent

Player:
    Parameters:
        SpawnPoints: 5

Tiles:
    Plain:
        Sprite: ../../GUI/Assets/Tiles/plain.png
        Symbol: .
        IsWalkable: true
        DefaultTile: true
    Water:
        Sprite: ../../GUI/Assets/Tiles/water.png
        Symbol: W
        IsWalkable: false
    Mountain:
        Sprite: ../../GUI/Assets/Tiles/rock.png
        Symbol: M
        IsWalkable: false
     
Actions:
    Spawn:
        Type: EntityAction
        Cooldown: 1
        Targets:
            Target:
                Type: EntityType
                ValidTargets: Fighter
                Conditions:
                    - "CanAfford(Source.Player, Target)"
                    - "CanSpawn(Source, Target)"
        Effects:
            - "SpawnRandom(Source, Target)"
            - "PayCost(Source.Player, Target)"

    # Attack Actions
    Attack:
        Type: EntityAction
        Cooldown: 1
        Targets:
            Target:
                Type: Entity
                ValidTargets: [Fighter, City]
                Conditions:
                    - "InRange(Source, Target, 1)"
        Effects:
            - "Attack(Target.Health, 25)"


    # Move Actions
    Move:
        Type: EntityAction
        Cooldown: 1
        Targets:
            Target:
                Type: Position
                Shape: Circle
                Size: 3
                Conditions:
                    - "IsWalkable(Target)"
        Effects:
            - "Move(Source, Target)"

Entities:
    City:
        Sprite: ../../GUI/Assets/Entities/castle.png
        Symbol: c
        LineOfSightRange: 5
        CanSpawn: Fighter
        Actions: [Spawn]
        Parameters:
            Health: 100

    Fighter:
        Sprite: ../../GUI/Assets/Entities/unit2.png
        Symbol: f
        LineOfSightRange: 6
        Actions: [Move, Attack]
        Parameters:
            Health: 100
        Cost:
            SpawnPoints: 3


Board:
    GenerationType: Manual
    Layout: |-
        M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
        M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  M
        M  M  .  .  .  .  .  .  c1 .  .  .  .  .  .  M  M
        M  M  .  .  .  .  .  .  .  .  .  .  M  M  .  M  M
        M  M  .  .  .  .  .  .  W  .  .  .  M  M  .  M  M
        M  M  .  .  .  .  .  .  W  .  .  .  .  .  .  .  M
        M  M  .  .  .  .  .  W  W  W  .  .  .  .  .  .  M
        M  .  .  .  .  .  W  W  W  W  W  .  .  .  .  .  M
        M  .  .  .  .  .  .  W  W  W  .  .  .  .  .  .  M
        M  .  .  .  .  .  .  .  W  .  .  .  .  .  .  .  M
        M  M  .  .  M  M  .  .  W  .  .  .  .  .  .  M  M
        M  M  .  .  M  M  .  .  .  .  .  .  .  .  .  M  M
        M  M  .  .  .  .  .  .  c0 .  .  .  .  .  .  M  M
        M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  M
        M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
                   
ForwardModel:
    LoseConditions: #If true: Player -> cant play
        NoHasCity:
        - "NoHasEntity(Source, City)"

    Trigger:
    - OnTick:
        ValidTargets: City
        Conditions:
            - "IsPlayerEntity(Source)"
        Effects:
            - "ModifyResource(Source.Player.SpawnPoints, 1)"

