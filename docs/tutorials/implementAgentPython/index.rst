.. _implement_python_agent:

.. role:: python(code)
   :language: python

#############################
Implementing Simple AI Agents (Python)
#############################

This page shows a few examples about how to create Python AI agents for Stratega. In all cases, a new AI agent must do the following, independently if the agent is created to play RTS or TBS games:

#. Extend the class **Agent** from `./include/Stratega/Agent/Agent.h <https://github.com/GAIGResearch/Stratega/blob/dev/Stratega/include/Stratega/Agent/Agent.h>`_ 
#. Override the method *computeAction*.   

The **computeAction** method is called every tick/turn of the game and it should return an ActionAssignment object with the list of actions the agent wants to execute.
The agent receives a copy of the GameState (with fog of war applied) and the forward model object for simulating the game. Depending on which game YAML-file was passed to 
Stratega, the forward model will behave accordingly.

++++++++++++++++
Do Nothing Agent
++++++++++++++++

The simplest possible agent that can be created for Stratega is one that does nothing at all. Let's create this agent step by step.

First, we need to import the python binary module produced from C++:

.. code-block:: python

    import stratega

We plan to override the virtual method *computeAction*. Hence, our class definition will look like this:

.. code-block:: python

    class DoNothingPythonAgent(stratega.Agent):
        def computeAction(self, state, forward_model, timer):
        

Now, let's add the code require to do... nothing. For the moment, let's assume we're playing an RTS game. Given that RTS games don't have turns, we just return an empty action assignment. 
As simple as shown in the next snippet:

.. code-block:: python

    class DoNothingPythonAgent(stratega.Agent):
        def computeAction(self, state, forward_model, timer):
                return stratega.ActionAssignment()

And this is all. This code is compatible with the RTS and TBS types of games in Stratega, so you can use this agent for any of them straightaway.

++++++++++++++++++++
Random Agent
++++++++++++++++++++

The second simplest agent that can be implemented is the Random Agent. This agent returns actions at random at every call of the *computeAction* function. The process to
set up this agent is the same as for the DoNothing agent (and any other agent in the framework). Hence, the class definition of the agent looks like this:


.. code-block:: python

    class RandomPythonAgent(stratega.Agent):
        def computeAction(self, state, forward_model, timer):


What differs from the DoNothing agent is the content of the *computeAction* method. First, in order to be able to select which action to return, we need the collection of actions
available in the current game state. This can be retrieved from the forward model objecct, by using the following call:

.. code-block:: python

    # Generate all available actions
	actions=forward_model.generate_actions(state, self.get_player_id())


In this code, 'actions' is a list of objects of type "Action" which contains all actions available in the current game state. Our next step is to choose a random element in this vector, 
which can be done by i) importing the random module to generate pseudo-random numbers; ii) picking one index from the random generator; and 3) picking the action with the given index:

.. code-block:: c++

    # Sample random action
    action_index=random.randint(0, actions.count()-1)
    action=actions.__getitem__(action_index)


Finally, we return an action assignment created from the action chosen at random. To do this, we use the static function *from_single_action* from the ActionAssignment class. This final
line completes the code for this function:



.. code-block:: python
    :linenos:
    import stratega

    class RandomPythonAgent(stratega.Agent):
        def computeAction(self, state, forward_model, timer):

            action_index=random.randint(0, actions.count()-1)
            action=actions.__getitem__(action_index)
            action_assignment=stratega.ActionAssignment.from_single_action(action)

            return action_assignment


Note that this code is compatible for both RTS and TBS games.