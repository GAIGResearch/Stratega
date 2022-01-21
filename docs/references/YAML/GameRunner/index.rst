.. _yaml-gamerunner:

Game Runner
===========

:Game Runner: Contais the option of the game runner.
:YAML Key: GameRunner

:Example:
.. code-block:: yaml

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

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - :ref:`AgentInitializationTime <yaml-game-agent-initialization-time>`
     - ``false``
     - ``map``
     - 
     - Defines the initialization agent budget options.
   * - :ref:`AgentComputationTime <yaml-game-agent-computation-time>`
     - ``false``
     - ``map``
     - 
     - Defines the computation agent budget options.