.. _yaml-game-agent-computation-time:

Agent Computation Time
===========

:Agent Computation Time: Contais the definition of the agent initialization budget.

:YAML Key: AgentComputationTime

:Example:
.. code-block:: yaml

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
   * - ``Enabled``
     - ``true``
     - ``bool``
     -  true/false
     - Enable the agent initialization budget.
   * - ``BudgetTimeMs``
     - ``true``
     - ``int``
     - 
     - Define the budget time of the computation time in milliseconds of the agent.
   * - ``DisqualificationTimeMs``
     - ``true``
     - ``int``
     - 
     - Define the max budget time to disqualify the agent in milliseconds. 
   * - ``MaxNumberWarnings``
     - ``true``
     - ``int``
     - 
     - Define the max number of warnings before the agent gets disqualificated.