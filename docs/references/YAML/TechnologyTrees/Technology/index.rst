.. _yaml-technology:

Technology
===========

:Technology: Contais the definition of the agent initialization budget.

:YAML Key: Technology

:Example:
.. code-block:: yaml

    Chemistry:
        Description: Enables sabotage technology tree
        Requirements: [Education]
        Cost:
            Research: 10
        Time: 5

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Description``
     - ``true``
     - ``string``
     -  
     - The description of the technology
   * - ``Requirements``
     - ``true``
     - ``list``
     - 
     - List of researched technologies needed to unlock this one.
   * - ``Cost``
     - ``true``
     - ``map``
     - 
     - Map of parameters to cost. Can be used to define a price to pay for this research.
   * - ``Time``
     - ``true``
     - ``int``
     - 
     - Time in game ticks needed to research this technology.