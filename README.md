# Stratega
Stratega, a general strategy games framework, has been designed to foster research on computational intelligence for strategy games. In contrast to other strategy game frameworks,Stratega allows to create a wide variety of turn-based and real-time strategy games using acommon API for agent development. Flexibility is achieved by utilising YAML-files to configure tiles, units, actions, and levels. Therefore, the user can design and run a variety of games to test developedagents without specifically adjusting it to the game being generated. The framework hasbeen built with a focus of statistical forward planning (SFP) agents. For this purpose, agents can access and modify game-states and use the forward model to simulate the outcome oftheir actions. While SFP agents have shown great flexibility in general game-playing, theirperformance is limited in case of complex state and action-spaces.

Included Game configurations:
  - No Name Game (most complex until now)
  - City Capturing
  - Kill The King
  - Protect The Base