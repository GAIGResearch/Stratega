.. _yaml-gamerenderer:

GameRenderer
===========

:GameRenderer: Contains the options of the GUI.
:YAML Key: GameRenderer

:Example:
.. code-block:: yaml

    GameRenderer:
        Resolution:
            Width: 800
            Height: 600
        Default Assets:
            FogOfWar: ../../assets/Tiles/notVisible.png
        GridIsIsometric: true
        TileSpriteOrigin:
            Width: 128
            Height: 112
        EntitySpriteOrigin:
            Width: 256
            Height: 360
        TileSpriteSize:
            Width: 256
            Height: 144
        EntitySpriteSize:
            Width: 512
            Height: 512
        #Optional:
        #Font: ../../assets/arial.ttf
        #OutlineShader: ../../assets/OutLine.frag

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Resolution``
     - ``true``
     - ``object``
     - 
     -  Defines the window resolution.
   * - ``Default Assets``
     - ``true``
     - ``object``
     - 
     - Defines the default assets.
   * - ``Font``
     - ``false``
     - ``string``
     - 
     - Path to the font used by the GUI.
   * - ``OutlineShader``
     - ``false``
     - ``string``
     -
     - Path to the outlineshader used to outline the entities.
   * - ``GridIsIsometric``
     - ``true``
     - ``bool``
     -
     - Define if the world is isometric.
   * - ``TileSpriteOrigin``
     - ``true``
     - ``int pair``
     -
     - Define the tile sprite origin.
   * - ``EntitySpriteOrigin``
     - ``true``
     - ``int pair``
     -
     - Define the entity sprite origin.
   * - ``TileSpriteSize``
     - ``true``
     - ``int pair``
     -
     - Define the tile sprite size.
   * - ``EntitySpriteSize``
     - ``true``
     - ``int pair``
     -
     - Define the entity sprite size.


Resolution
*********

:Resolution: Contains the width and height of the GUI.
:YAML Key: Resolution

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Width``
     - ``true``
     - ``int``
     - 
     - Window width
   * - ``Height``
     - ``true``
     - ``int``
     - 
     - Window height

Default Assets
**************

:Default Assets: Contains the definitions of the FogOfWar used by GUI.
:YAML Key: Default assets

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Selected``
     - ``true``
     - ``string``
     - 
     - Path to the selected sprite used to outline the entities

