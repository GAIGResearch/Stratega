.. _yaml-sampling-method:

SamplingMethod
===========

:SamplingMethod: Contais the definition of the sampling method used to sample the targets.

:YAML Key: SamplingMethod

:Example:
.. code-block:: yaml

    SamplingMethod: 
        Type: Neighbours
        Options:
            Shape: Square
            Size: 4
    
    SamplingMethod:
        Type: Dijkstra
        Options:
            SearchSize: 4
            AllowDiagonals: false


:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Type``
     - ``true``
     - ``type``
     - Neighbours / All / Dijkstra
     - Define the type of the target
   * - ``Options``
     - ``true``
     - ``type``
     -
     - Each sampling method contais a set of options to sample the targets.
