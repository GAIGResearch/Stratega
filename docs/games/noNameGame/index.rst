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
        - RandomAgent
        - HumanAgent

    Player:
        Actions: [Build, Research]
        CanSpawn: Buildings
        Parameters:
            Gold: 0
            Prod: 5

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

    Entities:
        City:
            Sprite: ../../GUI/Assets/Entities/castle.png
            Symbol: c
            LineOfSightRange: 5
            Actions: [Spawn]
            CanSpawn: [Worker]
            Cost:
                Prod: 10
            Parameters:
                Health: 800
                StorageCapacity: 50
                Range: 6

        Barracks:
            Sprite: ../../GUI/Assets/Entities/barracks.png
            LineOfSightRange: 3
            RequiredTechnology: Discipline
            Actions: [Spawn]
            CanSpawn: [Warrior, Archer]
            Cost:
                Prod: 20
            Parameters:
                Health: 500
                Range: 6

        MilitaryAcademy:
            Sprite: ../../GUI/Assets/Entities/military.png
            LineOfSightRange: 3
            RequiredTechnology: Mathematics
            Actions: [Spawn]
            CanSpawn: [Catapult]
            Cost:
                Prod: 40
            Parameters:
                Health: 500

        Foundry:
            Sprite: ../../GUI/Assets/Entities/foundry.png
            LineOfSightRange: 2
            RequiredTechnology: Metallurgy
            Actions: []
            Cost:
                Prod: 50
            Parameters:
                Health: 300
            
        Workshop:
            Sprite: ../../GUI/Assets/Entities/storage.png
            LineOfSightRange: 2
            RequiredTechnology: Apprenticeship
            Actions: []
            Cost:
                Prod: 50
            Parameters:
                Health: 300

        Storage:
            Sprite: ../../GUI/Assets/Entities/castle.png
            LineOfSightRange: 1
            RequiredTechnology: Pottery
            Actions: []
            Cost:
                Prod: 40
            Parameters:
                Cost: 40
                Health: 200
                StorageCapacity: 50

        Worker:
            Sprite: ../../GUI/Assets/Entities/unit7.png
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
            Sprite: ../../GUI/Assets/Entities/unit2.png
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
                Health: 150
            
        Archer:
            Sprite: ../../GUI/Assets/Entities/unit3.png
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
                Health: 100
            
        Catapult:
            Sprite: ../../GUI/Assets/Entities/unit4.png
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
            Sprite: ../../GUI/Assets/Entities/gold_chest.png
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
                    Conditions:
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
                    Shape: Circle
                    Size: 1 # Target.Speed
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
                    ValidTargets: All  # I'd say this isn't necessary (if not present, default == All)
                    Conditions:
                        - "CanResearch(Source, Target)"
                        - "CanAfford(Source, Target)"
            TriggerComplete:
                - "HasElapsedTick(Target.Time)"
            OnStart:
                - "PayCost(Source, Target)"
            OnComplete:
                - "Research(Source, Target)"

    
        Mine:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: GoldVein
                    Conditions:
                        - "InRange(Source, Target, 2)"
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
                    Shape: Square
                    Size: 2
                    Conditions:
                        - "IsWalkable(TargetPosition)"

            Effects:
                - "SpawnEntity(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source.Player, EntityTypeTarget)"

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
                    Shape: Circle
                    Size: 1
                    Conditions:
                        - "IsWalkable(TargetPosition)"

            Effects:
                - "SpawnEntity(Source, EntityTypeTarget, TargetPosition)"
                - "PayCost(Source, EntityTypeTarget)"

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

    #Action categories
    GameDescription:
        Type: CombatGame
        Actions:
            Move: [Move]
            Research: [Research]
            Gather: [Mine]
            Spawn: [Spawn, Build]
            Attack: [Attack]