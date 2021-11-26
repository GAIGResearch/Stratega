.. _yaml-entity:

Entity
===========

:Entity: Contais the definition the entity type.

:YAML Key: Entity

:Example:
.. code-block:: yaml

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

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Sprite``
     - ``true``
     - ``string``
     -  
     - Path to the sprite used to render the entity in the GUI mode.
   * - ``Symbol``
     - ``true``
     - ``char``
     - 
     - Symbol used to parse the map board.
   * - ``LineOfSightRange``
     - ``true``
     - ``int``
     - 
     - Line of sight of the unit. Used to render the fog of war.
   * - ``Actions``
     - ``false``
     - ``list``
     - 
     - List of actions this entity can generate.
   * - ``CanSpawn``
     - ``false``
     - ``list``
     - 
     - List of entities this entity can spawn.
   * - ``Cost``
     - ``false``
     - ``map``
     - 
     - Map of parameter to cost, used to know the price to spawn this entity.
   * - ``Parameters``
     - ``false``
     - ``map``
     - 
     - List of parameters of the entity. Optionally you can define the min and max value of the parameter by defining it with the following template "[min,default,max]".