###############
Protect the Base
###############

Yaml paths:

.. code-block:: c++

    /gameConfigs/TBS/ProtectTheBase.yaml

.. only:: html

   .. figure:: ../../images/protectTheBaseTBS.gif

++++++++++++++++++++
Description
++++++++++++++++++++

Simple combat TBS game with basic rules. Two players should gather gold and spawn fighters to destroy the enemy base.
Similar to City capturing but with resource gathering.
The gold amount in the mines is limited, take care and dont run off of gold.

++++++++++++++++++++
YAML
++++++++++++++++++++
.. code-block:: yaml

   GameConfig:
        Type: TBS
        ActionTimer: 10s
        RoundLimit: 100

    Agents:
        - RandomAgent
        - HumanAgent

    GameRenderer:
        Resolution:
            Width: 800
            Height: 600
        Default Assets:
            Selected: ../../assets/Tiles/selected.png
            FogOfWar: ../../assets/Tiles/notVisible.png
        #Optional:
        #Font: ../../assets/arial.ttf
        #OutlineShader: ../../assets/OutLine.frag

    Tiles:
        Plain:
            Sprite: ../../assets/Tiles/plain.png
            Symbol: .
            IsWalkable: true
            DefaultTile: true
        Water:
            Sprite: ../../assets/Tiles/water.png
            Symbol: W
            IsWalkable: false
        Mountain:
            Sprite: ../../assets/Tiles/rock.png
            Symbol: M
            IsWalkable: false
            BlocksSight: true
        Forest:
            Sprite: ../../assets/Tiles/forest.png
            Symbol: F
            IsWalkable: true
        
    Actions:
        # Spawn Actions
        SpawnMiner:
            Type: EntityAction
            Cooldown: 1
            Preconditions:
                - "ResourceGreaterEqual(Source.Gold, 50)"
            Targets:
                Target:
                    Type: Position
                    SamplingMethod:
                        Type: Neighbours
                        Options:
                            Shape: Circle
                            Size: 3
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "SpawnEntity(Source, Miner, Target)"
                - "ModifyResource(Source.Gold, -50)"

        SpawnFighter:
            Type: EntityAction
            Cooldown: 1
            Preconditions:
                - "ResourceGreaterEqual(Source.Gold, 100)"
            Targets:
                Target:
                    Type: Position
                    SamplingMethod: 
                        Type: Neighbours
                        Options:
                            Shape: Circle
                            Size: 3
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "SpawnEntity(Source, Fighter, Target)"
                - "ModifyResource(Source.Gold, -100)"

        # Attack Actions
        Attack:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Attackable
                    SamplingMethod: 
                        Type: Neighbours
                        Options:
                            Shape: Circle
                            Size: 4
                    Conditions:
                        - "DifferentPlayer(Source, Target)"
            Effects:
                - "Attack(Target.Health, Source.AttackDamage)"

        # Move Actions
        Move:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Position
                    SamplingMethod: 
                        Type: Dijkstra
                        Options:
                            SearchSize: 3
                            AllowDiagonals: false
                    #    Type: Neighbours
                    #    Options:
                    #        Shape: Circle
                    #        Size: 1
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "Move(Source, Target)"

        # Resource Actions
        Mine:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: GoldVein
                    SamplingMethod: 
                        Type: Neighbours
                        Options:
                            Shape: Square
                            Size: 1
                    Conditions:
                        - "ResourceGreaterEqual(Target.Gold, 40)"
            Effects:
                - "Transfer(Target.Gold, Source.Gold, 40)"

        Deposit:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Base
                    SamplingMethod: 
                        Type: Neighbours
                        Options:
                            Shape: Square
                            Size: 1
            Effects:
                - "Transfer(Source.Gold, Target.Gold, Source.Gold)"

    Entities:
        Base:
            Sprite: ../../assets/Entities/castle.png
            Symbol: b
            LineOfSightRange: 5
            Actions: [SpawnMiner, SpawnFighter]
            Parameters:
                Gold: 500
                Health: 100

        Miner:
            Sprite: ../../assets/Entities/unit_5.png
            Symbol: m
            LineOfSightRange: 4
            Actions: [Move, Mine, Deposit]
            Parameters:
                Gold: 0
                Health: 20

        Fighter:
            Sprite: ../../assets/Entities/unit_2.png
            Symbol: f
            LineOfSightRange: 6
            Parameters:
                AttackDamage: 10
                Health: 40
            Actions: [Move, Attack]

        GoldVein:
            Sprite: ../../assets/Entities/gold_chest.png
            Symbol: g
            LineOfSightRange: 6
            Actions: []
            Parameters:
                Gold: 200

    EntityGroups:
        Attackable: [Base, Fighter, Miner]

    Board:
        GenerationType: Manual
        Layout: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  .  .  .  .  .  .  .  .  g  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  .  .  g  .  .  .  .  .  W  W  .  .  .  .  .  .  b1 .  .  .  .  M  M  .  .  .  .  .  M  M  M
            M  .  .  g  .  .  .  .  .  .  W  W  W  W  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  .  .  g  .  .  g  g  g  .  W  W  W  W  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  M  M  g  .  .  .  .  g  .  .  .  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  .  W  W  W  W  W
            M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W
            M  .  .  .  g  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  g  .  .  .  .  .  .  .  .  .  .  .  .  .  b0 .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  M  M  g  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  .  .  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
                    
    ForwardModel:
        LoseConditions: #If true: Player -> cant play
            HasNoCity:
            - "HasNoEntity(Source, Base)"

    #Action categories
    GameDescription:
        Type: CombatGame
        Actions:
            Move: [Move]
            Gather: [Mine, Deposit]
            Spawn: [SpawnFighter, SpawnMiner]
            Attack: [Attack]
        Entities:
            Base: [Base]
            Building: [Base]
            Spawner: [Base]
            Unit: [Miner, Fighter]
            Fighter: [Fighter]
            NoFighter: [Miner]
            Melee: [Fighter]