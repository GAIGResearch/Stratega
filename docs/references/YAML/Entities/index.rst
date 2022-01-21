.. _yaml-entities:

Entities
===========

:Entities: Contains the definition of the entities used in the game.
:YAML Key: Entities

:Example:
.. code-block:: yaml

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
                Health: [0,200,300] #[min, default, max]
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


:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - :ref:`Entity <yaml-entity>`
     - ``false``
     - ``map``
     - 
     - Entity type definition
