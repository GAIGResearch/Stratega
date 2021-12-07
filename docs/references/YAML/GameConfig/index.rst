.. _yaml-gameconfig:

GameConfig
===========

:GameConfig: Contains the type of game and a set of options.
:YAML Key: GameConfig

:Example:
.. code-block:: yaml

    GameConfig:
        Type: TBS
        ActionTimer: 10s
        RoundLimit: 100
        Parameters:
          FlagsCaptured: [0,0,10]
          CurrentSpawnedFlagsNumber: [0,0,5]

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Type``
     - ``true``
     - ``string``
     - ``TBS/RTS``
     - Defines the game type  
   * - ``RoundLimit``
     - ``false``
     - ``int``
     - 
     - Defines the turns limit in ticks
   * - ``Parameters``
     - ``false``
     - ``map``
     - 
     - List of parameters of the state. Optionally you can define the min and max value of the parameter by defining it with the following template "[min,default,max]".