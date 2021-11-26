.. _yaml-entity-groups:

Entity Groups
===========

:Entity Groups: Contains the list of custom groups of entities. Can be used categorize the entities in groups.
:YAML Key: EntityGroups

:Example:
.. code-block:: yaml

 EntityGroups:
    Units: [Worker, Warrior]
    Buildings: [City, Barracks]
    Attackable: [City, Barracks, Worker, Warrior]
    #Add any custom group here...

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Custom group name``
     - ``false``
     - ``list /single value``
     - 
     - Defines the list of entities that form part of a categorized group.

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
