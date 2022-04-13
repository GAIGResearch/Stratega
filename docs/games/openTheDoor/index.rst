###############
Open the door
###############

Yaml paths:

.. code-block:: c++

    /gameConfigs/TBS/Test/OpenTheDoor.yaml

.. only:: html

   .. figure:: ../../images/openTheDoorTBS.gif

++++++++++++++++++++
Description
++++++++++++++++++++

TBS game in which two players starts with a single searcher unit and a door. The goal of the game is to find a key in the labyrinth, pick it up, equip it and open the door. 

++++++++++++++++++++
YAML
++++++++++++++++++++
.. code-block:: yaml

    GameConfig:
        Type: TBS
        RoundLimit: 100
        FogOfWar: true

    Agents:
        - HumanAgent
        - OSLAAgent

    Player:
        Parameters:
            DoorsOpened: [0,0,10]

    GameRenderer:
        Resolution:
            Width: 800
            Height: 600
        Default Assets:
            FogOfWar: ../../../assets/Tiles/notVisible.png
        GridIsIsometric: true
        TileSpriteOrigin:
            Width: 128
            Height: 112
        EntitySpriteOrigin:
            Width: 256
            Height: 360
        TileSpriteSize:
            Width: 256
            Height: 144
        EntitySpriteSize:
            Width: 512
            Height: 512
        #Optional:
        #Font: ../../assets/arial.ttf
        #OutlineShader: ../../assets/OutLine.frag

    Tiles:
        Plain:
            Sprite: ../../../assets/Tiles/plain.png
            Symbol: .
            IsWalkable: true
            DefaultTile: true
        Mountain:
            Sprite: ../../../assets/Tiles/rock.png
            Symbol: M
            IsWalkable: false
            BlocksSight: true
            
    Board:
        GenerationType: Manual
        Layout: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  s1 .  .  M  .  .  .  .  .  M  .  M  .  .  .  .  .  M  .  .  .  M
            M  .  M  .  M  .  M  M  M  .  M  .  M  .  M  M  M  .  M  .  M  .  M
            M  .  M  .  .  .  M  .  M  .  .  .  .  .  M  .  M  .  .  .  M  .  M
            M  .  M  .  M  .  M  .  M  .  M  .  M  .  M  .  M  .  M  .  M  .  M
            M  .  M  M  M  M  M  .  M  .  M  .  M  .  M  .  M  M  M  M  M  .  M
            M  .  M  .  .  .  .  .  M  .  M  .  M  .  M  .  .  .  .  .  M  .  M
            M  .  M  .  M  .  M  .  M  k  M  .  M  k  M  .  M  .  M  .  M  .  M
            M  .  M  .  M  .  M  .  M  M  M  .  M  M  M  .  M  .  M  .  M  .  M
            M  .  .  .  M  d1 M  .  .  .  .  .  .  .  .  .  M  d0 M  .  .  s0 M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
    Actions:
        Move:
            Type: EntityAction
            Preconditions:
                - "ResourceGreaterEqual(Source.MovementPoints, 1)"
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
                        - "IsNotOccupiedGrid(Target, Source)"
            Effects:
                - "Move(Source, Target)"
                - "ModifyResource(Source.MovementPoints, -1)"

        PickUp:
            Type: EntityAction
            Preconditions:
                - "HasNotInventoryFull(Source)"
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Object
                    SamplingMethod:
                        Type: Neighbours
                        Options:
                            Shape: AllPositions
                    Conditions:
                        - "InRange(Source, Target, Source.ActionRange)"
            Effects:
                - "PickUp(Source, Target)"

        EquipObject:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Object
                    Conditions:
                        - "CanEquipObject(Source, Target)"
            Effects:
                - "EquipObject(Source, Target)"

        UnEquipObject:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: SlotObject
            Effects:
                - "UnEquipObject(Source, Target)"

        #UseObject:
        #    Type: EntityAction
        #    Cooldown: 1
        #    Targets:
        #        Target:
        #            Type: Object
        #    Effects:
        #        - "UseObject(Source, Target)"

        #DropObject:
        #    Type: EntityAction
        #    Cooldown: 1
        #    Targets:
        #        Object:
        #            Type: Object
        #        Position:
        #            Type: Position
        #            SamplingMethod: 
        #                Type: Dijkstra
        #                Options:
        #                    SearchSize: 3
        #                    AllowDiagonals: false
        #    Effects:
        #        - "DropObject(Source, Object, Position)"

        UseSlotObject:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Object:
                    Type: SlotObject
                TargetDoor:
                    Type: Entity
                    ValidTargets: Door
                    SamplingMethod:
                        Type: Neighbours
                        Options:
                            Shape: AllPositions
                    Conditions:
                        - "InRange(Source, TargetDoor, Source.ActionRange)"
            Effects:
                - "UseSlotObject(Source, Object)"

        
    Entities:
        Searcher:
            Sprite: ../../../assets/Entities/unit_2.png
            Symbol: s
            LineOfSightRange: 2
            Actions: [Move, PickUp, EquipObject, UnEquipObject, UseSlotObject]
            InventorySize: 4
            Slots: [Left hand, Right hand]
            Parameters:
                Health: 200
                ActionRange: 2
                MovementPoints: 2

        Door:
            Sprite: ../../assets/Entities/wonder.png
            Symbol: d
            LineOfSightRange: 2

    Objects:
        Key:
        Sprite: ../../../assets/Entities/key.png
        Symbol: k
        CanEquip: [Searcher] #Who can equip the object in the slots
        SlotsUse: [Right hand] #empty == cant be equiped in any slot

        #Source object , Target entity
        #OnEquip: 
            #Conditions: #conditions executed when the object is added to a slot
            #Effects: #effects executed when the object is added to a slot
        #OnAddedInventory: 
            #Conditions: #conditions executed when the object is added to the inventory
            #Effects: #effects executed when the object is added to the inventory
        OnUseSlot: 
            #Conditions: #conditions executed when the object is used in the slot
            Effects: #effects executed when the object is used in the slot
                - "ModifyResource(Target.Player.DoorsOpened, 1)"
        #OnUseInventory:
            #Conditions: #effects executed when the object is used in the inventory
            #Effects: #effects executed when the object is used in the inventory
            #    - "ModifyResource(Target.Health, Source.HealAmount)"

        #OnTickActions: [] #Execute when in on slot or inventory
        #Parameters:
        #    HealAmount: 20
        #    HealRange: 2

    EntityGroups:
        Object: [Key]
        Entities: [Searcher, Door]

    ForwardModel:
        WinConditions: #If true: Player -> win game
            Flags:
            - "ResourceGreaterEqual(Source.DoorsOpened, 1)"

        Trigger:
            - OnTick:
                Type: Entity
                ValidTargets: Searcher
                Effects:
                    - "SetToMaximum(Source.MovementPoints)"

    #Action categories
    GameDescription:
        Type: CombatGame
        Actions:
            Move: [Move]
        Entities:
            Unit: [Searcher]

    GameRunner:
        AgentInitializationTime:
            Enabled: false
            BudgetTimeMs: 50
            DisqualificationTimeMs: 70
        AgentComputationTime:
            Enabled: false
            BudgetTimeMs: 1000
            DisqualificationTimeMs: 70
            MaxNumberWarnings: 5