.. _yaml-player:

Player
===========

:Player: Contains the list of parameters, actions and entities that each player can spawn
:YAML Key: Player

:Example:
.. code-block:: yaml

  Player:
      Actions: [Build, Research]
      CanSpawn: Buildings
      Parameters:
          Gold: [0,0,1000] #[min, default, max]
          Prod: 50

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Actions``
     - ``false``
     - ``list /single value``
     - 
     - Defines the list of actions the player can execute on the game.
   * - ``CanSpawn``
     - ``false``
     - ``list /single value``
     - 
     - Defines the list of entities the player can spawn on the game. If is not defined, the player can spawn "All" the entities.
   * - ``Parameters``
     - ``false``
     - ``map``
     - 
     - Defines the list of parameters of each player. Optionally you can define the min and max value of the parameter by defining it with the following template "[min,default,max]".
