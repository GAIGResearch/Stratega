####################
Installation
####################

.. note::
    We currently do not offer any binaries. Instead, the source code has to be compiled manually. We are working on simplifying the installation of the framework.

++++++++++++++++++++++++++++++++
Windows using Visual Studio 2019
++++++++++++++++++++++++++++++++
Stratega is compiled using CMake. Luckily Visual Studio already provides a simple way to do this. 

#. Install Visual Studio 2019 Community from `here <https://visualstudio.microsoft.com/downloads/>`_
#. Clone Stratega from `here <https://github.com/GAIGResearch/Stratega/>`_ (use the master branch)
#. In Visual Studio open the folder (not project) containing the Stratega source code
#. Execute **Project->Generate Cache**, Visual Studio may automatically do this when opening the folder
#. Execute **Build->Build All** to generate the executables
#. Under **Select Startup Item...** choose GUI.exe and press **F5** to run it

++++++++++++++++++++++++++++++++
Mac OS
++++++++++++++++++++++++++++++++
.. note::
    Coming Soon


++++++++++++++++++++++++++++++++++
Compiling Documentation (Advanced)
++++++++++++++++++++++++++++++++++
Stratega generates documentation using `Doxygen <https://www.doxygen.nl/download.html>`_. By default, the generation of documentation is **disabled** (see  `CMakeLists.txt <https://github.com/GAIGResearch/Stratega/blob/master/CMakeLists.txt>`_, STRATEGA_BUILD_DOCS set to OFF). If you wish to generate documentation, you must first install Doxygen and then set STRATEGA_BUILD_DOCS to ON. If, despite installing Doxygen, the **Generate Cache** process still gives any error, follow this complete `installation instructions <https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/>`_ for potentially missing dependencies.
