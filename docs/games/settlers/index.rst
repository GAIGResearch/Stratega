###############
Settlers
###############

Yaml paths:

.. code-block:: c++

    /gameConfigs/TBS/Settlers.yaml
    /gameConfigs/RTS/Settlers.yaml

.. only:: html

   .. figure:: ../../images/settlersRTS.gif

++++++++++++++++++++
Description
++++++++++++++++++++

Is the most complex combat TBS and RTS game that Stratega has until now. 
This game has two different technology trees, one focused in enabling the user to spawn more entities and the second one to increase the resource gathering.
Research is the key to win the game, either by spawning a spy and sabotage the enemy or building a wonder.
Try to dont starve, each entity consume food!!

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

    Player:
        Actions: [Research]
        CanSpawn: Buildings
        Parameters:
            Food: 500
            Gold: 0
            Iron: 50  
            Wood: 100
            Bricks: 50
            Research: 10000

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
            BlocksSight: true
        Forest:
            Sprite: ../../GUI/Assets/Tiles/forest.png
            Symbol: F
            IsWalkable: true
        Ore:
            Sprite: ../../GUI/Assets/Tiles/ore.png
            Symbol: O
            IsWalkable: true
        Animals:
            Sprite: ../../GUI/Assets/Tiles/animals.png
            Symbol: A
            IsWalkable: true
        Clay:
            Sprite: ../../GUI/Assets/Tiles/clay.png
            Symbol: C
            IsWalkable: true
        Gold:
            Sprite: ../../GUI/Assets/Tiles/gold.png
            Symbol: G
            IsWalkable: true

    Entities:
        City:
            Sprite: ../../GUI/Assets/Entities/castle.png
            Symbol: c
            LineOfSightRange: 5
            Actions: [Spawn]
            CanSpawn: [Villager, Builder, Spy, Architect]
            Cost:
            Parameters:
                Health: 800
                Range: 6

        Wonder:
            Sprite: ../../GUI/Assets/Entities/wonder.png
            LineOfSightRange: 5
            Actions: []
            Cost:
                Wood: 2000
                Iron: 2000
                Food: 2000
                Bricks: 2000
            Parameters:
                Health: 200
                Range: 6

        University:
            Sprite: ../../GUI/Assets/Entities/university.png
            LineOfSightRange: 5
            RequiredTechnology: Education
            Actions: []
            Cost:
            Parameters:
                Health: 200
                Range: 6

        Outpost:
            Sprite: ../../GUI/Assets/Entities/tower.png
            LineOfSightRange: 8
            RequiredTechnology: BasicStructure
            Actions: []
            Cost:
            Parameters:
                Health: 200
                Range: 7
                Time: 200

        GuardTower:
            Sprite: ../../GUI/Assets/Entities/guardTower.png
            LineOfSightRange: 4
            RequiredTechnology: GuardTower
            Actions: []
            Cost:
            Parameters:
                Health: 200
                Range: 6
                Time: 200

        Farm:
            Sprite: ../../GUI/Assets/Entities/farm.png
            LineOfSightRange: 3
            Cost:
            Parameters:
                Health: 800
                Range: 6

        Masonry:
            Sprite: ../../GUI/Assets/Entities/masonry.png
            LineOfSightRange: 3
            Cost:
            Parameters:
                Health: 800
                Range: 6

        Saw:
            Sprite: ../../GUI/Assets/Entities/saw.png
            LineOfSightRange: 3
            Cost:
            Parameters:
                Health: 800
                Range: 6

        IronMine:
            Sprite: ../../GUI/Assets/Entities/mine.png
            LineOfSightRange: 3
            Cost:
            Parameters:
                Health: 800
                Range: 6

        GoldMine:
            Sprite: ../../GUI/Assets/Entities/mine2.png
            LineOfSightRange: 3
            Cost:
            Parameters:
                Health: 800
                Range: 6
            
        Villager:
            Sprite: ../../GUI/Assets/Entities/unit7.png
            LineOfSightRange: 3
            Actions: [Move, CutWood, MineIron, MineGold, FarmLivestock, MakeBrick]
            Cost:
            Parameters:
                Speed: 1
                Range: 3
                WoodCutting: 0.1
                BrickMaking: 0.1
                LivestockFarming: 0.1
                IronMining: 0.1
                GoldMining: 0.1
            
        Builder:
            Sprite: ../../GUI/Assets/Entities/unit10.png
            LineOfSightRange: 3
            CanSpawn: [Farm, GoldMine, IronMine, Saw, Masonry, University, Outpost, GuardTower]
            Actions: [Move, BuildFarm, BuildGoldMine, BuildIronMine, BuildSaw, BuildMansory, BuildUniversity, BuildGuardTower, BuildOutpost]
            Cost:
            Parameters:
                Speed: 1
                Range: 6

        Spy:
            Sprite: ../../GUI/Assets/Entities/unit9.png
            LineOfSightRange: 5
            Actions: [Move, Sabotage]
            RequiredTechnology: Sabotage
            Cost:
            Parameters:
                Health: 20
                Speed: 1
                Range: 6

        Architect:
            Sprite: ../../GUI/Assets/Entities/unit11.png
            LineOfSightRange: 3
            RequiredTechnology: Architecture
            CanSpawn: [Wonder]
            Actions: [Move, BuildWonder]
            Cost:
            Parameters:
                Speed: 1
                Range: 6

    EntityGroups:
        Units: [Villager, Builder, Spy, Architect]
        Buildings: [City, Farm, IronMine, GoldMine, Outpost, GuardTower, University, Wonder]
        Attackable: [Farm, IronMine, GoldMine, Outpost, GuardTower, University, Wonder]

    Actions:
        # Attack Actions
        Sabotage:
            Type: EntityAction
            Cooldown: 100
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Attackable
                    Conditions:
                        - "InRange(Source, Target, Source.Range)"
            Effects:
                - "AttackProbability(Target.Health, 1000, 50)"
                - "Remove(Source)"
        Attack:
            Type: EntityAction
            Cooldown: 100
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Spy
                    Conditions:
                        - "InRange(Source, Target, Source.Range)"
            Effects:
                - "AttackProbability(Target.Health, 1000, 20)"

        # Move Actions
        Move:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Position
                    Shape: Circle
                    Size: 100 # Target.Speed
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "Move(Source, Target)"

        MineIron:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: IronMine
                    Conditions:
                        - "InRange(Source, Target, 3)"
            Effects:
                - "ModifyResource(Source.Player.Iron, Source.IronMining)"

        MineGold:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: GoldMine
                    Conditions:
                        - "InRange(Source, Target, 3)"
            Effects:
                - "ModifyResource(Source.Player.Gold, Source.GoldMining)"

        FarmLivestock:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Farm
                    Conditions:
                        - "InRange(Source, Target, 3)"
            Effects:
                - "ModifyResource(Source.Player.Food, Source.LivestockFarming)"

        CutWood:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Saw
                    Conditions:
                        - "InRange(Source, Target, 3)"
            Effects:
                - "ModifyResource(Source.Player.Wood, Source.WoodCutting)"

        MakeBrick:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Masonry
                    Conditions:
                        - "InRange(Source, Target, 3)"
            Effects:
                - "ModifyResource(Source.Player.Bricks, Source.BrickMaking)"

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
                    Shape: Square
                    Size: 4
                    Conditions:
                        - "IsWalkable(TargetPosition)"

            Effects:
                - "SpawnEntity(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source.Player, EntityTypeTarget)"

        BuildIronMine:
            Type: EntityAction
            Cooldown: 1
            Targets:
                EntityTypeTarget:
                    Type: EntityType
                    ValidTargets: IronMine
                    Conditions:
                        - "CanAfford(Source.Player, EntityTypeTarget)"
                        - "CanSpawn(Source, EntityTypeTarget)"

                TargetPosition:
                    Type: Position
                    Shape: Square
                    Size: 4
                    Conditions:
                        - "IsWalkable(TargetPosition)"
                        - "IsTileType(TargetPosition, Ore)"
            Effects:
                - "SpawnEntityGrid(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source.Player, EntityTypeTarget)"

        BuildGoldMine:
            Type: EntityAction
            Cooldown: 1
            Targets:
                EntityTypeTarget:
                    Type: EntityType
                    ValidTargets: GoldMine
                    Conditions:
                        - "CanAfford(Source.Player, EntityTypeTarget)"
                        - "CanSpawn(Source, EntityTypeTarget)"

                TargetPosition:
                    Type: Position
                    Shape: Square
                    Size: 4
                    Conditions:
                        - "IsWalkable(TargetPosition)"
                        - "IsTileType(TargetPosition, Gold)"
            Effects:
                - "SpawnEntityGrid(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source.Player, EntityTypeTarget)"

        BuildFarm:
            Type: EntityAction
            Cooldown: 1
            Targets:
                EntityTypeTarget:
                    Type: EntityType
                    ValidTargets: Farm
                    Conditions:
                        - "CanAfford(Source.Player, EntityTypeTarget)"
                        - "CanSpawn(Source, EntityTypeTarget)"

                TargetPosition:
                    Type: Position
                    Shape: Square
                    Size: 4
                    Conditions:
                        - "IsWalkable(TargetPosition)"
                        - "IsTileType(TargetPosition,Animals)"
            Effects:
                - "SpawnEntityGrid(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source.Player, EntityTypeTarget)"

        BuildUniversity:
            Type: EntityAction
            Cooldown: 1
            Targets:
                EntityTypeTarget:
                    Type: EntityType
                    ValidTargets: University
                    Conditions:
                        - "CanAfford(Source.Player, EntityTypeTarget)"
                        - "CanSpawn(Source, EntityTypeTarget)"

                TargetPosition:
                    Type: Position
                    Shape: Square
                    Size: 4
                    Conditions:
                        - "IsWalkable(TargetPosition)"
                        - "IsTileType(TargetPosition, Plain)"
            Effects:
                - "SpawnEntityGrid(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source.Player, EntityTypeTarget)"

        BuildMansory:
            Type: EntityAction
            Cooldown: 1
            Targets:
                EntityTypeTarget:
                    Type: EntityType
                    ValidTargets: Masonry
                    Conditions:
                        - "CanAfford(Source.Player, EntityTypeTarget)"
                        - "CanSpawn(Source, EntityTypeTarget)"

                TargetPosition:
                    Type: Position
                    Shape: Square
                    Size: 4
                    Conditions:
                        - "IsWalkable(TargetPosition)"
                        - "IsTileType(TargetPosition, Clay)"
            Effects:
                - "SpawnEntityGrid(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source.Player, EntityTypeTarget)"

        BuildSaw:
            Type: EntityAction
            Cooldown: 1
            Targets:
                EntityTypeTarget:
                    Type: EntityType
                    ValidTargets: Saw
                    Conditions:
                        - "CanAfford(Source.Player, EntityTypeTarget)"
                        - "CanSpawn(Source, EntityTypeTarget)"

                TargetPosition:
                    Type: Position
                    Shape: Square
                    Size: 4
                    Conditions:
                        - "IsWalkable(TargetPosition)"
                        - "IsTileType(TargetPosition, Forest)"
            Effects:
                - "SpawnEntityGrid(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source.Player, EntityTypeTarget)"

        BuildOutpost:
            Type: EntityAction
            Cooldown: 1
            Targets:
                EntityTypeTarget:
                    Type: EntityType
                    ValidTargets: Outpost
                    Conditions:
                        - "CanAfford(Source.Player, EntityTypeTarget)"
                        - "CanSpawn(Source, EntityTypeTarget)"

                TargetPosition:
                    Type: Position
                    Shape: Square
                    Size: 4
                    Conditions:
                        - "IsWalkable(TargetPosition)"
            Effects:
                - "SpawnEntityGrid(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source.Player, EntityTypeTarget)"

        BuildGuardTower:
            Type: EntityAction
            Cooldown: 1
            Targets:
                EntityTypeTarget:
                    Type: EntityType
                    ValidTargets: GuardTower
                    Conditions:
                        - "CanAfford(Source.Player, EntityTypeTarget)"
                        - "CanSpawn(Source, EntityTypeTarget)"

                TargetPosition:
                    Type: Position
                    Shape: Square
                    Size: 4
                    Conditions:
                        - "IsWalkable(TargetPosition)"
            Effects:
                - "SpawnEntityGrid(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source.Player, EntityTypeTarget)"

        BuildWonder:
            Type: EntityAction
            Cooldown: 1
            Targets:
                EntityTypeTarget:
                    Type: EntityType
                    ValidTargets: Wonder
                    Conditions:
                        - "CanAfford(Source.Player, EntityTypeTarget)"
                        - "CanSpawn(Source, EntityTypeTarget)"

                TargetPosition:
                    Type: Position
                    Shape: Square
                    Size: 4
                    Conditions:
                        - "IsWalkable(TargetPosition)"
    #                   - "IsTileType(Forest)"
            TriggerComplete:
                - "HasElapsedTick(200)"
            OnStart:
                - "PayCost(Source.Player, EntityTypeTarget)"
            OnComplete:
                - "SpawnEntityGrid(Source, EntityTypeTarget, TargetPosition)"

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

    TechnologyTrees:
        TechnologyTree:
            Education:
                Description: Enables advance research
                Cost:
                    Research: 5
                Time: 200
            Chemistry:
                Description: Enables sabotage technology tree
                Requirements: [Education]
                Cost:
                    Research: 10
                Time: 500
            Architecture:
                Description: Enables architect spawn
                Requirements: [Education]
                Cost:
                    Research: 15
                Time: 500
            BasicStructure:
                Description: Enable outpost
                Requirements: [Education]
                Cost:
                    Research: 15
                Time: 500
            SiegeEngineers:
                Description:  Enables sabotage technology tree
                Requirements: [Architecture]
                Cost:
                    Research: 20
                Time: 600
            Sabotage:
                Description:  Enables sabotage
                Requirements: [SiegeEngineers, Chemistry]
                Cost:
                    Research: 20
                Time: 700
            GuardTower:
                Description:  Enables guard tower spawn
                Requirements: [Architecture, BasicStructure]
                Cost:
                    Research: 30
                Time: 600
            Wonder:
                Description:  Enables wonder building
                Requirements: [Architecture, BasicStructure]
                Cost:
                    Research: 100
                Time: 4000

        ResourceTree:
            BetterTools:
                Description: Enables resource technologies
                Cost:
                    Research: 5
                Time: 200
            HeavyPlow:
                Description: Increase food production
                Requirements: [BetterTools]
                Cost:
                    Research: 20
                Time: 400
            BowSaw:
                Description: Increase wood production
                Requirements: [BetterTools]
                Cost:
                    Research: 20
                Time: 400
            OreShaftMining:
                Description: Increase ore production
                Requirements: [BetterTools]
                Cost:
                    Research: 25
                Time: 400
            ClayShaftMining:
                Description:  Increase clay production
                Requirements: [BetterTools]
                Cost:
                    Research: 25
                Time: 400
            CropRotation:
                Description:  Increase food production
                Requirements: [HeavyPlow]
                Cost:
                    Research: 30
                Time: 600
            TwoManSaw:
                Description:  Enables guard tower spawn
                Requirements: [BowSaw]
                Cost:
                    Research: 30
                Time: 600


    Board:
        GenerationType: Manual
        Layout: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  F  F  F  F  F  F  M
            M  .  .  c1 .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  F  F  F  F  M
            M  .  .  .  .  .  .  .  .  .  .  A  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  A  A  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  .  .  .  M
            M  .  F  F  F  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  .  .  .  M
            M  .  F  F  F  .  .  .  .  .  C  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  .  .  .  .  .  M
            M  .  F  F  F  .  .  .  .  .  C  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  .  .  .  .  M
            M  .  .  .  .  .  .  A  .  .  C  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  .  .  .  M
            M  .  .  A  .  .  .  .  .  .  C  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  C  W  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  .  .  .  .  .  .  .  .  .  .  W  W  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            W  W  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  O  M
            W  W  W  W  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  .  .  .  .  .  .  .  .  .  .  O  O  O  M
            W  W  W  W  W  .  .  .  .  .  .  .  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M  M  M  M  M
            M  M  M  W  W  .  .  .  .  .  .  .  .  .  .  .  .  G  G  .  .  .  .  .  .  .  .  .  .  .  W  W  M  M  M
            M  M  M  W  W  W  .  .  .  .  .  .  .  .  .  .  .  G  G  .  .  .  .  .  .  .  .  .  .  W  W  W  M  M  M
            M  M  M  M  M  O  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  .  .  .  .  .  .  .  .  W  W  M  M  M
            M  M  M  M  M  O  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W  .  .  .  .  .  .  .  W  W  W  W  W
            M  M  M  M  M  O  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  .  .  .  .  .  .  .  .  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  .  .  .  .  .  .  .  .  .  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  C  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  C  .  .  .  .  .  .  .  .  M
            M  .  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  C  .  .  .  .  .  .  .  .  M
            M  .  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  C  .  .  .  .  .  .  .  .  M
            M  .  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  C  .  .  .  .  .  .  .  .  M
            M  .  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  A  .  .  M
            M  .  .  .  W  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  A  A  .  .  M
            M  .  .  .  W  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  W  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  W  W  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  A  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  W  W  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  W  W  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  A  F  F  F  .  .  .  .  .  .  M
            M  F  F  F  F  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  F  F  F  .  .  .  .  .  .  M
            M  F  F  F  F  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  F  F  F  .  .  .  .  c0 .  M
            M  F  F  F  F  F  F  F  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
                   
    ForwardModel:
        WinConditions: #If true: Player -> win game
            UnitAlive:
            - "HasEntity(Source, Wonder)"

        LoseConditions: #If true: Player -> cant play
            HasResource:
            - "ResourceLowerEqual(Source.Food, 0)"

        Trigger:
       #Global resource compsumption
            - OnTick:
                ValidTargets: Buildings
                Conditions:
                    - "IsPlayerEntity(Source)"
                Effects:
                    - "ModifyResource(Source.Player.Food, -0.04)"
            - OnTick:
                ValidTargets: Units
                Conditions:
                    - "IsPlayerEntity(Source)"
                Effects:
                    - "ModifyResource(Source.Player.Food, -0.04)"

            - OnTick:
                ValidTargets: University
                Conditions:
                    - "IsPlayerEntity(Source)"
                Effects:
                    - "ModifyResource(Source.Player.Research, +0.6)"
        #Upgrade villagers resuorce collection
            #Food
            - OnTick:
                ValidTargets: Villager
                Conditions:
                    - "IsPlayerEntity(Source)"
                    - "HasResearched(Source.Player, HeavyPlow)"
                Effects:
                    - "ChangeResource(Source.LivestockFarming, 0.11)"

            - OnTick:
                ValidTargets: Villager
                Conditions:
                    - "IsPlayerEntity(Source)"
                    - "HasResearched(Source.Player, CropRotation)"
                Effects:
                    - "ChangeResource(Source.LivestockFarming, 0.13)"
            #Wood
            - OnTick:
                ValidTargets: Villager
                Conditions:
                    - "IsPlayerEntity(Source)"
                    - "HasResearched(Source.Player, BowSaw)"
                Effects:
                    - "ChangeResource(Source.WoodCutting, 0.12)"

            - OnTick:
                ValidTargets: Villager
                Conditions:
                    - "IsPlayerEntity(Source)"
                    - "HasResearched(Source.Player, TwoManSaw)"
                Effects:
                    - "ChangeResource(Source.WoodCutting, 0.13)"
            #Ore
            - OnTick:
                ValidTargets: Villager
                Conditions:
                    - "IsPlayerEntity(Source)"
                    - "HasResearched(Source.Player, OreShaftMining)"
                Effects:
                    - "ChangeResource(Source.IronMining, 0.125)"
            #Clay
            - OnTick:
                ValidTargets: Villager
                Conditions:
                    - "IsPlayerEntity(Source)"
                    - "HasResearched(Source.Player, ClayShaftMining)"
                Effects:
                    - "ChangeResource(Source.BrickMaking, 0.125)"

    #Action categories
    GameDescription:
        Type: CombatGame
        Actions:
            Move: [Move]
            Research: [Research]
            Gather: [MineIron, MineGold, FarmLivestock, CutWood, MakeBrick]
            Spawn: [Spawn, BuildIronMine, BuildGoldMine, BuildFarm, BuildUniversity, BuildMansory, BuildSaw, BuildOutpost, BuildGuardTower, BuildWonder]
            Attack: [Sabotage, Attack]