.. _implement_advanced_agents:

.. role:: cpp(code)
   :language: c++

#############################
Advanced Agents
#############################

Stratega includes the implementation of two Statistical Forward Planning (SFP) agents: MCTS and RHEA. SFP agents use the forward model to sample 
future states and make decisions based on the statistics gathered from them. For more information and a description of these two algorithms, 
please check the following papers:

#. Monte Carlo Tree Search (MCTS):  (`A Survey of Monte Carlo Tree Search Methods <https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=6145622>`_).
#. Rolling Horizon Evolutionary Algorithm (RHEA): (`Rolling Horizon Evolutionary Algorithms for General Video Game Playing <https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=9357946>`_).

Stratega's implementation of these agents can be found in the following files:

#. Monte Carlo Tree Search (MCTS): in 'include/Stratega/Agent/TreeSearchAgents/' (there are also corresponding .cpp files in 'src/Stratega/Agent/TreeSearchAgents/'):
    * `MCTSAgent.h/cpp <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Agent/MCTSAgent/MCTSAgent.h>`_: Main entry point for the agent, which implements the init and computeAction methods as specified in :ref:`Implementing Simple AI Agents <implement_cpp_agent>`.
    * `MCTSNode.h/cpp <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Agent/MCTSAgent/MCTSNode.h>`_: Implements a node for MCTS. It contains the main functions for the MCTS algorithm (tree selection, simulation, back-propagation and UCT).  
    * `MCTSParameters.h/cpp <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Agent/MCTSAgent/MCTSParameters.h>`_: Parameters of the agent. For more information, see below how to provide these via YAML.
#. Rolling Horizon Evolutionary Algorithm (RHEA): in 'include/Stratega/Agent/RHEAAgent/' (plus .cpp in 'src/Stratega/Agent/RHEAAgent/')
    * `RHEAAgent.h/cpp <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Agent/RHEAAgent/RHEAAgent.h>`_: Main agent class for the RHEA agent, with the implementation of the methods init and computeAction, as specified in :ref:`Implementing Simple AI Agents <implement_cpp_agent>`. It also manages the population of individuals for the evolutionary algorithm.
    * `RHEAGenome.h/cpp <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Agent/RHEAAgent/RHEAGenome.h>`_: Implements an individual for RHEA, which all the main functions such as mutation, crossover and evaluation.
    * `RHEAParameters.h/cpp <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Agent/RHEAAgent/RHEAParameters.h>`_: Parameters of the agent. For more information, see below how to provide these via YAML.


+++++++++++++++++++
Agent Parameters
+++++++++++++++++++

These two agents have in common that they both use a class to specify parameters of the agent. These are 'MCTSParameters' and 'RHEAParameters', and both classes extend a base class
'AgentParameter', which can be used by any agent in the framework. The AgentParameter class holds different type of information:

#. Budget information: see section below.
#. Player ID of the agent (needs to be set up in the agent's init method).
#. A `State Heuristic <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Agent/Heuristic/StateHeuristic.h>`_, used to evaluate states found during search. State heuristic is a pure virtual function that requires the implementation of two functions: *evaluateGameState* (that provies a numerical value given a game state) and *getName* (used for printing purposes). Stratega provides some huristics as examples:
    * `MinimizeDistanceHeuristic <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/src/Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.cpp>`_, which aims to minimize the distance of the player's units to the opponent's.
    * `AbstractHeuristic <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/src/Stratega/Agent/Heuristic/AbstractHeuristic.cpp>`_, which aims to maximize the difference between the player's units properties and those of the opponent.
#. An Opponent Model, used to determine which actions the opponent could take when simulating actions with the forward model. This is a class of type `BaseActionScript <https://github.com/GAIGResearch/Stratega/blob/dev/Stratega/include/Stratega/Agent/ActionScript/BaseActionScript.h>`_, which requires the implementation of a function called *getAction*. This function returns an action for a given game state, and it can be used as a way to incorporate domain knowledge into the game.
#. A Portfolio, which is a collection of 'BaseActionScript' objects. This is useful for implementing portfolio-based search methods. See, for instance, this `paper <https://www.diego-perez.net/papers/PortfolioStrategaCEC2021.pdf>`_.

Each class, 'MCTSParameters' and 'RHEAParameters', extend the base class with parameters that are specific to each algorithm. For instance, 
for `MCTS <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Agent/MCTSAgent/MCTSParameters.h>`_ you can specify 
values for the tree policy balancing constant C, the playout length or if Monte Carlo rollouts are active. For 
`RHEA <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/include/Stratega/Agent/RHEAAgent/RHEAParameters.h>`_ you can specify the population size,
mutation rate or tournament size for selection.

MCTS and RHEA also show how parameters can be set and initialized. Stratega allows you to do this in two independent ways:

Initialization via code
***********************

As mentioned previously, each agent can implement a method called *init*, which is called only once at the start of each game. The MCTS agent uses this method to initialize the 
members from AgentParameters that regard the player ID, heuristic and opponent model:


.. code-block:: c++
    :linenos:

    void MCTSAgent::init(GameState initialState, const ForwardModel& forwardModel, Timer timer)
    {
        parameters_.PLAYER_ID = getPlayerID();
        if (parameters_.heuristic == nullptr)
            parameters_.heuristic = std::make_unique<AbstractHeuristic>(initialState);
        parameters_.opponentModel = std::make_shared<RandomActionScript>();
    }


Note that this function provides access to the initial state of the game and also to a forward model, which can be used during the initialization. The third parameter, *timer*,
can be used to control how long is left until the function must complete (using *timer.exceededMaxTime()*). 


Initialization via YAML
***********************

Parameters can also be sent, via YAML, to Stratega agents. This is useful to avoid recompiling the code to try different agent configurations. Although the parameters that can
be provided to an agent are **not** limited to those defined in AgentParameters, MCTS and RHEA use this feature to initialize the variables from 
'MCTSParameters' and 'RHEAParameters', respecitively. In order to implement the reading of YAML parameters, the following function 'decode' is added to 
RHEAParameters.h (analogously for MCTSParameters.h):


.. code-block:: c++
    :linenos:

    namespace YAML
    {
        template<>
        struct convert<SGA::RHEAParameters>
        {
            /// <summary>
            /// Reads Agent generic (via call rhs.decode(node)) and RHEA-specific parameters received in a YAML node.
            /// Add lines to this method to capture more parameters.
            /// </summary>
            /// <param name="node">YAML Node with the parameters for the agent.</param>
            /// <param name="rhs">Parameters object to be modified with the contents of the YAML node.</param>
            /// <returns>True if there was no problem.</returns>
            static bool decode(const Node& node, SGA::RHEAParameters& rhs)
            {
                rhs.decode(node);
                rhs.popSize = node["PopSize"].as<size_t>(rhs.popSize);
                rhs.individualLength = node["IndLength"].as<size_t>(rhs.individualLength);
                rhs.mutationRate = node["MutationRate"].as<double>(rhs.mutationRate);
                rhs.tournamentSize = node["TournamentSize"].as<int>(rhs.tournamentSize);
                rhs.elitism = node["Elitism"].as<bool>(rhs.elitism);
                rhs.continuePreviousSearch = node["ContPreviousSearch"].as<bool>(rhs.continuePreviousSearch);
                rhs.mutateBestN = node["MutateBestN"].as<size_t>(rhs.mutateBestN);
                return true;
            }
        };
    }
    

In YAML, these parameters can be sent to the agents following the format below:


.. code-block:: yaml
    :linenos:

    Agents:
      - RHEAAgent:
            MutationRate: 0.9
            TournamentSize: 3
            Elitism: true


Finally, in order for Stratega to link the YAML and the 'decode' function, an agent able to receive parameters must be defined in 
`AgentFactory <https://github.com/GAIGResearch/Stratega/blob/dev/src/stratega/src/Stratega/Agent/AgentFactory.cpp>`_ as follows, using the name of the class
that contains the 'decode' function:

.. code-block:: c++
    :linenos:

    AgentFactory& AgentFactory::getDefaultFactory()
	{
		static AgentFactory factory;

		// No parameters:
        factory.registerAgent<DoNothingAgent>("DoNothingAgent");

        // Agent with parameters:
        factory.registerAgent<RHEAAgent, RHEAParameters>("RHEAAgent");

		return factory;
	}


+++++++++++++++++++
Specifying a Budget
+++++++++++++++++++

Initializing an agent and returning an action in Stratega are timed events. Time limits for each of these functions are specified in the 'GameRunner'
section of the game's YAML file:


.. code-block:: yaml
    :linenos:

    GameRunner:
        AgentInitializationTime:
            Enabled: true
            BudgetTimeMs: 1000
            DisqualificationTimeMs: 5000
        AgentComputationTime:
            Enabled: true
            BudgetTimeMs: 100
            DisqualificationTimeMs: 1000
            MaxNumberWarnings: 5


In the example above, the agent's initalization time is 1 second, and the time to return an action is 100ms. The meaning and consequences of these time
limits defer depending on the game being a TBS or an RTS:

#. In TBS, an action returned after the allowed time will be ignored, causing the end of the turn for the player. If the time returned exceeds this time, the agent will receive a warning. After 5 warnings (according to the setting above) or if the return time is higher than 'DisqualificationTimeMs', the agent will be disqualified from the game. 
#. In RTS, game states are updated with actions from the player every 'BudgetTimeMs' ms. Actions returned after this time are ignored (no action is applied), and no disqualifications are applied.


The MCTS and RHEA agents allow to determine the stopping criteria of the algorithms to return an action in different ways. This is implemented in AgentParameter, in the 
field 'budgetType', of type 'enum class BudgetType', and there are three configurations: TIME (aims to return before timeout), ITERATIONS (specifies a concrete number of 
algorithm iterations to run) and FMCALLS (which determines the number of calls to the *advance* function of the forward model). This budget type can also be initialized via
YAML, as well as the specific counts for the different modes. See the examples below for the three implemented stopping criteria:

.. tabs::

    .. code-tab:: yaml Iterations

        Agents:
            - MCTSAgent:
                Budget: ITERATIONS
                Iterations: 200

    .. code-tab:: yaml FM Calls

        Agents:
            - MCTSAgent:
                Budget: FMCALLS
                FmCalls: 2000

    .. code-tab:: yaml Time

        Agents:
            - MCTSAgent:
                Budget: TIME
                PercTime: 0.9


Each budget type should define a secondary parameter that refers to the actual amount of budget available. Iterations and forward model calls are straightforward, while 
'PercTime' indicates the proportion of time that is considered to be over. For instance, a value of 0.9 means that the function *isBudgetOver()* in the AgentParameter class
will return 'true' when 90% of the thinking time has been consumed. This is useful to adjust computation effort to an amount that avoids overtiming.

Both MCTS and RHEA reset their budget counters at the beginning of their *computeAction* methods:

.. code-block:: c++
    :linenos:

    namespace SGA
    {
	    ActionAssignment BFSAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
	    {
		    parameters_.resetCounters(timer);
            //...
        }
    }

which allows agents to call the function *isBudgetOver()* in the AgentParameter class to determine if the budget is over, independently of the type of budget 
set in the parameters. For instance, the main loop of MCTS, in 'MCTSNode.cpp', uses this function to determine the stopping criteria:

.. code-block:: c++
    :linenos:

    void MCTSNode::searchMCTS(ForwardModel& forwardModel, MCTSParameters& params, boost::mt19937& randomGenerator) {
        // stop in case the budget is over.
        while (!params.isBudgetOver()) {
            MCTSNode* selected = treePolicy(forwardModel, params, randomGenerator);
            double delta = selected->rollOut(forwardModel, params, randomGenerator);
            backUp(selected, delta);
            params.currentIterations++; 
        }
    }
