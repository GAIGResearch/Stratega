###############
City Capturing
###############

Yaml paths:

.. code-block:: c++

    /gameConfigs/TBS/CityCapturing.yaml

.. only:: html

   .. figure:: ../../images/cityCapturingTBS.gif

++++++++++++++++++++
Description
++++++++++++++++++++

Simple combat TBS game with basic rules. Two players should fight to conquer all the cities of the map.
Use fighters to kill the enemy conquers and defend your city.

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

    Player:
        Parameters:
            Gold: 5

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
        Forest:
            Sprite: ../../assets/Tiles/forest.png
            Symbol: F
            IsWalkable: true
        
    Actions:
        Spawn:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: EntityType
                    ValidTargets: Units
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
                    SamplingMethod:
                    #   Type: Dijkstra
                    #   Options:
                    #       SearchSize: 3
                    #       AllowDiagonals: false
                        Type: Neighbours
                        Options:
                            Shape: Circle
                            Size: 3
                    ValidTargets: [Conquerer, Fighter]
                    Conditions:
                        - "DifferentPlayer(Source, Target)"
            Effects:
                - "Attack(Target.Health, 25)"

        # Capturing
        Capture:
            Type: EntityAction
            Targets:
                Target:
                    Type: Entity
                    SamplingMethod: 
                        Type: Dijkstra
                        Options:
                            SearchSize: 1
                            AllowDiagonals: false
                    #    Type: Neighbours
                    #    Options:
                    #        Shape: Circle
                    #        Size: 1
                    ValidTargets: City
            Effects:
                - "ChangeOwner(Target, Source.Player)"
                - "Remove(Source)"

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
                    Conditions:
                    - "IsWalkable(Target)"
            Effects:
                - "Move(Source, Target)"


    Entities:
        City:
            Sprite: ../../assets/Entities/castle.png
            Symbol: c
            LineOfSightRange: 5
            CanSpawn: Units
            Actions: [Spawn]

        Conquerer:
            Sprite: ../../assets/Entities/unit_5.png
            Symbol: s
            LineOfSightRange: 4
            Actions: [Move, Capture]
            Parameters:
                Health: 25
            Cost:
                Gold: 6

        Fighter:
            Sprite: ../../assets/Entities/unit_2.png
            Symbol: f
            LineOfSightRange: 6
            Actions: [Move, Attack]
            Parameters:
                Health: 100
            Cost:
                Gold: 5

    EntityGroups:
        Units: [Fighter, Conquerer]

    Board:
        GenerationType: Manual
        #Path or definition of multiple maps
        #Maps: ../../../gameConfigs/TBS/maps.yaml
        #Maps:
        #    Map1: |-
        #        M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  c1 .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  W  .  W  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  W  W  c  W  W  .  .  c  .  .  M
        #        M  .  .  .  .  .  .  W  .  W  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  c0 .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
        #
        #    Map2: |-
        #        M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  c1 .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  W  .  W  .  .  .  .  .  .  M
        #        M  .  .  c  .  .  W  W  .  W  W  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  W  .  W  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  c0 .  .  .  .  .  .  .  M
        #        M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
        #        M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
        #Layout: Map1
        Layout: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  c1 .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  W  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  W  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  W  .  .  .  .  M
            M  .  .  .  .  .  .  W  .  W  .  W  .  .  .  .  M
            M  .  .  c  .  .  W  W  c  W  W  .  .  c  .  .  M
            M  .  .  .  .  .  .  W  .  W  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  c0 .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
                    
    ForwardModel:
        LoseConditions: #If true: Player -> cant play
            HasNoCity:
            - "HasNoEntity(Source, City)"

        Trigger:
            - OnTick:
                ValidTargets: City
                Conditions:
                    - "IsPlayerEntity(Source)"
                Effects:
                    - "ModifyResource(Source.Player.Gold, 1)"

    #Action categories
    GameDescription:
        Type: CombatGame
        Actions:
            Move: [Move]
            Spawn: [Spawn]
            Attack: [Attack, Capture]
        Entities:
            Base: [City]
            Unit: [Fighter, Conquerer]
            Fighter: [Fighter]
            NoFighter: [Conquerer]