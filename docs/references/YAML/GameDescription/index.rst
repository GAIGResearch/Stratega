.. _yaml-game-description:

Game Description
===========

:Game Description: Contais the definition of the game description and represents a grouping of action and entities types into categories. The catogories are defined default and can be used to know by the agent the type of each action.

:YAML Key: GameDescription

:Example:
.. code-block:: yaml

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

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - :ref:`Actions <yaml-game-description-actions>`
     - ``false``
     - ``map``
     - 
     - Defines the actions mapped to a category.
   * - :ref:`Entities <yaml-game-description-entities>`
     - ``false``
     - ``map``
     - 
     - Defines the entities mapped to a category.

:Use examples:
.. code-block:: yaml

  #Used in player or entity CanSpawn definition instead of a single entity.
  Player:
      Actions: ...
      CanSpawn: Buildings
      Parameters:
          ...

  City:
      Sprite: ...
      Symbol: c
      LineOfSightRange: 5
      Actions: ...
      CanSpawn: Units
      Cost:
          ...
      Parameters:
          ...

  #Used in the ValidTargets of each target instead of a single entity.
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
