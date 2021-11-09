###############
Basic RTS/TBS
###############

Yaml paths:

.. code-block:: c++

    /gameConfigs/TBS/BasicTBS.yaml
    /gameConfigs/RTS/BasicRTS.yaml

.. only:: html

   .. figure:: ../../images/noNameGameTBS.gif

++++++++++++++++++++
Description
++++++++++++++++++++

Complex combat TBS and RTS game. Two players should fight and destroy the city of the opponent.
This game have only one technology tree focused in enabling the spawn of new structures and units.
There are two resources used to spawn new sructures or units, production points and gold. Gold can be mined from mine deposits and production points are produced by cities.

++++++++++++++++++++
YAML
++++++++++++++++++++
.. code-block:: yaml

    GameConfig:
        Type: TBS
        ActionTimer: 10s
        RoundLimit: 100

    Agents:
        - HumanAgent
        - OSLAAgent

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
        Actions: [Build, Research]
        CanSpawn: Buildings
        Parameters:
            Gold: 0
            Prod: 5
            Score: 0

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

    Entities:
        City:
            Sprite: ../../assets/Entities/castle.png
            Symbol: c
            LineOfSightRange: 5
            Actions: [Spawn]
            CanSpawn: [Worker]
            Cost:
                Prod: 10
            Parameters:
                Health: 200
                StorageCapacity: 50
                Range: 6

        Barracks:
            Sprite: ../../assets/Entities/barracks.png
            LineOfSightRange: 3
            RequiredTechnology: Discipline
            Actions: [Spawn]
            CanSpawn: [Warrior, Archer]
            Cost:
                Prod: 20
            Parameters:
                Health: 100
                Range: 6

        MilitaryAcademy:
            Sprite: ../../assets/Entities/military.png
            LineOfSightRange: 3
            RequiredTechnology: Mathematics
            Actions: [Spawn]
            CanSpawn: [Catapult]
            Cost:
                Prod: 40
            Parameters:
                Health: 100

        Foundry:
            Sprite: ../../assets/Entities/foundry.png
            LineOfSightRange: 2
            RequiredTechnology: Metallurgy
            Actions: []
            Cost:
                Prod: 50
            Parameters:
                Health: 50
                
        Workshop:
            Sprite: ../../assets/Entities/storage.png
            LineOfSightRange: 2
            RequiredTechnology: Apprenticeship
            Actions: []
            Cost:
                Prod: 50
            Parameters:
                Health: 50

        Storage:
            Sprite: ../../assets/Entities/castle.png
            LineOfSightRange: 1
            RequiredTechnology: Pottery
            Actions: []
            Cost:
                Prod: 40
            Parameters:
                Cost: 40
                Health: 50
                StorageCapacity: 50

        Worker:
            Sprite: ../../assets/Entities/unit_5.png
            LineOfSightRange: 3
            Actions: [Move, Mine]
            Cost:
                Prod: 5
            Time: 5
            Parameters:
                ResourceCapacity: 10
                Armor: 5
                Speed: 1
                WeaponStrength: 5
                Health: 50
                Range: 3
                
        Warrior:
            Sprite: ../../assets/Entities/unit_2.png
            LineOfSightRange: 3
            Actions: [Move, Attack]
            RequiredTechnology: Bronze Working
            Cost:
                Prod: 10
            Parameters:
                Armor: 20
                Speed: 1
                Range: 3
                WeaponStrength: 25
                Morale: 10
                Health: 50
                
        Archer:
            Sprite: ../../assets/Entities/unit_3.png
            LineOfSightRange: 3
            Actions: [Move, Attack]
            RequiredTechnology: Archery
            Cost:
                Prod: 15
            Parameters:
                Armor: 15
                Speed: 1
                Range: 6
                WeaponStrength: 15
                ReloadTime: 2
                Morale: 10
                Health: 30
                
        Catapult:
            Sprite: ../../assets/Entities/unit_4.png
            LineOfSightRange: 3
            Actions: [Move, Attack]
            RequiredTechnology: Engineering
            Cost:
                Prod: 20
            Parameters:
                Armor: 10
                Speed: 1
                Range: 15
                WeaponStrength: 40
                ReloadTime: 3
                Morale: 5
                Health: 60

        GoldVein:
            Sprite: ../../assets/Entities/gold_chest.png
            Symbol: g
            LineOfSightRange: 6
            Actions: []
            Parameters:
                Gold: 200

    EntityGroups:
        Units: [Worker, Warrior, Archer, Catapult]
        Buildings: [City, Barracks, MilitaryAcademy, Foundry, Workshop, Storage]
        Attackable: [City, Barracks, MilitaryAcademy, Foundry, Workshop, Storage, Worker, Warrior, Archer, Catapult]

    Actions:
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
                            Shape: AllPositions
                    Conditions:
                        - "DifferentPlayer(Source, Target)"
                        - "InRange(Source, Target, Source.Range)"
            Effects:
                - "Attack(Target.Health, Source.WeaponStrength)"

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
                            SearchSize: 2
                            AllowDiagonals: false
                    #    Type: Neighbours
                    #    Options:
                    #        Shape: Circle
                    #        Size: 1
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "Move(Source, Target)"

        #Research
        Research:
            Type: PlayerAction
            Cooldown: 0
            Targets:
                Target:
                    Type: Technology
                    ValidTargets: All  
                    Conditions:
                        - "CanResearch(Source, Target)"
                        - "CanAfford(Source, Target)"
            TriggerComplete:
                - "HasElapsedTick(Target.Time)"
            OnStart:
                - "PayCost(Source, Target)"
            OnComplete:
                - "Research(Source, Target)"
                - "ModifyResource(Source.Score, 10)"

        
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
                            Shape: Circle
                            Size: 2
                    Conditions:
                        - "ResourceGreaterEqual(Target.Gold, 40)"
            Effects:
                - "Transfer(Target.Gold, Source.Player.Gold, 40)"

        #Spawn
        Spawn:
            Type: EntityAction
            Cooldown: 0
            Targets:
                EntityTypeTarget:
                    Type: EntityType
                    ValidTargets: Units
                    Conditions:
                        - "CanSpawn(Source, EntityTypeTarget)"
                        - "CanAfford(Source.Player, EntityTypeTarget)"
                TargetPosition:
                    Type: Position
                    SamplingMethod: 
                        Type: Neighbours
                        Options:
                            Shape: Circle
                            Size: 2
                    Conditions:
                        - "IsWalkable(TargetPosition)"

            Effects:
                - "SpawnEntity(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source.Player, EntityTypeTarget)"
                - "ModifyResource(Source.Player.Score, 2)"

        #Build
        Build:
            Type: PlayerAction
            Cooldown: 1
            Targets:
                EntityTypeTarget:
                    Type: EntityType
                    ValidTargets: Buildings
                    Conditions:
                        - "CanAfford(Source, EntityTypeTarget)"
                        - "CanSpawn(Source, EntityTypeTarget)"

                TargetPosition:
                    Type: Position
                    SamplingMethod:
                        Type: Neighbours
                        Options:
                            Shape: AllPositions
                    Conditions:
                        - "IsWalkable(TargetPosition)"

            Effects:
                - "SpawnEntity(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source, EntityTypeTarget)"
                - "ModifyResource(Source.Score, 5)"

    TechnologyTrees:
        SingleTree:
            Mining:
                Description: Base technology
                Cost:
                    Prod: 10
                Time: 2
            Discipline:
                Description: Enables barracks construction.
                Requirements: [Mining]
                Cost:
                    Prod: 15
                Time: 2
            Pottery:
                Description: Allows to construct a Storage.
                Requirements: [Mining]
                Cost:
                    Prod: 10
                Time: 2
            Mathematics:
                Description: Here you can build a military academy
                Requirements: [Mining]
                Cost:
                    Prod: 15
                Time: 2
            Archery:
                Description: Here you can spawn archers.
                Requirements: [Discipline]
                Cost:
                    Gold: 20
                Time: 5
            Bronze Working:
                Description: Here you can spawn warriors.
                Requirements: [Discipline]
                Cost:
                    Gold: 20
                Time: 5
            Apprenticeship:
                Description: Here you can build a workshop.
                Requirements: [Pottery]
                Cost:
                    Gold: 30
                Time: 5
            Metallurgy:
                Description: Here you can build a foundry.
                Requirements: [Apprenticeship]
                Cost:
                    Gold: 40
                Time: 8
            Engineering:
                Description: Here you can spawn a catapult.
                Requirements: [Mathematics]
                Cost:
                    Gold: 30
                Time: 5
                

    Board:
        GenerationType: Manual
        Layout: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  .  .  .  .  .  .  .  .  g  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  .  .  .  .  F  F  F  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  F  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  .  .  g  .  .  .  .  .  W  W  .  .  .  .  .  .  c1 .  .  .  .  M  M  .  .  .  .  .  M  M  M
            M  .  .  g  .  .  .  .  .  .  W  W  W  W  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  .  .  g  .  .  g  g  g  .  W  W  W  W  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  M  M  g  .  .  .  .  g  .  .  .  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  .  W  W  W  W  W
            M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W
            M  .  .  .  g  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  g  .  .  F  F  F  F  .  .  .  .  .  .  .  c0 .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  M  M  g  .  .  .  F  F  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  .  .  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
                    
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
                    - "ModifyResource(Source.Player.Prod, 1)"
            - OnTick:
                ValidTargets: Workshop
                Conditions:
                    - "IsPlayerEntity(Source)"
                Effects:
                    - "ModifyResource(Source.Player.Prod, 2)"
            - OnTick:
                ValidTargets: Foundry
                Conditions:
                    - "IsPlayerEntity(Source)"
                    - "ResourceGreaterEqual(Source.Player.Gold, 2)"
                Effects:
                    - "ModifyResource(Source.Player.Gold, -2)"
                    - "ModifyResource(Source.Player.Prod, 4)"

    #Game Description
    GameDescription:
        Type: CombatGame
        Actions:
            Move: [Move]
            Research: [Research]
            Gather: [Mine]
            Spawn: [Spawn, Build]
            Attack: [Attack]
        Entities:
            Base: [City]
            Building: [City, Barracks, MilitaryAcademy, Foundry, Workshop, Storage]
            Spawner: [City, Barracks, MilitaryAcademy]
            Unit: [Worker, Warrior, Archer, Catapult]
            Fighter: [Warrior, Archer, Catapult]
            NoFighter: [Worker]
            Melee: [Warrior]
            Ranged: [Archer, Catapult]