.. _yaml-forwardmodel-trigger:

Trigger
===========

:Trigger: Contais the list of triggers.

:YAML Key: Trigger

:Example:
.. code-block:: yaml

    Trigger:
    #Global resource compsumption
        - OnTick:
            ValidTargets: Buildings
            Conditions:
                - "IsPlayerEntity(Source)"
            Effects:
                - "ModifyResource(Source.Player.Food, -1)"
        - OnTick:
            ValidTargets: Units
            Conditions:
                - "IsPlayerEntity(Source)"
            Effects:
                - "ModifyResource(Source.Player.Food, -1)"

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``ValidTargets``
     - ``true``
     - ``bool``
     -  true/false
     - List of valid targets affected by this OnTick action.
   * - ``Conditions``
     - ``true``
     - ``int``
     - 
     - Defines the list of conditions. If all the condition returns true the effects will be executed.
   * - ``Effects``
     - ``true``
     - ``int``
     - 
     - Define the list of effects executed on each target.