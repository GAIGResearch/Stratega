.. _yaml-game-agent-initialization-time:

Agent Initialization Time
===========

:Agent Initialization Time: Contais the definition of the agent initialization budget.

:YAML Key: AgentInitializationTime

:Example:
.. code-block:: yaml

    AgentInitializationTime:
        Enabled: false
        BudgetTimeMs: 50
        DisqualificationTimeMs: 70

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
     - Define the budget time of the initialization time in milliseconds of the agent.
   * - ``DisqualificationTimeMs``
     - ``true``
     - ``int``
     - 
     - Define the max budget time in milliseconds to disqualify the agent.