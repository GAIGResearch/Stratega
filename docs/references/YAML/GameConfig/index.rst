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