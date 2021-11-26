.. _yaml-agents:

Agents
===========

:Agents: Contains the list of agents that will play the game. Optionally agent parameters can be defined inside the same agents.
:YAML Key: Agents

:Example:
.. code-block:: yaml

    Agents:
        - RandomAgent
        - HumanAgent

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Agents``
     - ``true``
     - ``list``
     - ``HumanAgent \ MCTSAgent \ ...``
     - Defines the list of agents that will play the game. Optionally you can use the same definition to define the options of the agent.
AgentsParameters
****************

:AgentsParameters: Contains the options of each agent previously defined in C++. There are two examples of the RHEAAgent and MCTSAgent. 
:YAML Key: "Agent Name"

:Example:
.. code-block:: yaml

    Agents:
    - HumanAgent
    - MCTSAgent:
        FmCalls: 100
        Budget: TIME
        RolloutLength: 10

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``FmCalls``
     - ``false``
     - ``int``
     - 
     - The maximum number of forward model calls.
   * - ``Iterations``
     - ``false``
     - ``int``
     - 
     - Maximum number of iterations for a decision making round.
   * - ``PercTime``
     - ``false``
     - ``double``
     - 
     - Proportion of the timer that must have passed before the function isBudgetOver triggers.
   * - ``Budget``
     - ``false``
     - ``string``
     - ``TIME \ FMCALLS \ ITERATIONS``
     - Type of budget for the agent's decision making.

