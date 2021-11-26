.. _yaml-technology-trees:

Technology Trees
===========

:Technology Trees: Contains the list of technology trees that can be used to group different technologies.
:YAML Key: TechnologyTrees

:Example:
.. code-block:: yaml

    TechnologyTrees:
        TechnologyTree:
            Education:
                Description: Enables advance research
                Cost:
                    Research: 5
                Time: 2
            Chemistry:
                Description: Enables sabotage technology tree
                Requirements: [Education]
                Cost:
                    Research: 10
                Time: 5
            #Define here any technology

        ResourceTree:
            BetterTools:
                Description: Enables resource technologies
                Cost:
                    Research: 5
                Time: 2
            HeavyPlow:
                Description: Increase food production
                Requirements: [BetterTools]
                Cost:
                    Research: 20
                Time: 4

        #Define here any technology tree


:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - :ref:`Technology <yaml-technology>`
     - ``false``
     - ``map``
     - 
     - Define a list of technologies of the technology tree.
