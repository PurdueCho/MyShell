# IMPORTANT
You cannot just copy this for your class project. All basic skeleton sources belongs to Purdue C.S (cs252)
Also, all codes what I additionally implemented is stored in Purdue central git as well.
Your Professor and TA can track everything! So do not copy this for your work. It is plagiarism.

# MyShell
The goal of this project is to build a shell interpreter which combines behavior from common shells including bash and csh.

# What I implemented
Part 1: Parsing and Executing Commands
- Part 1A: Lex and Yacc - Accepting more complex commands
- Part 1B: Executing commands <br>
  1B.1: Simple command process creation and execution <br>
  1B.2: File redirection<br>
  1B.3: Pipes <br>
  1B.4: isatty() <br>

Part 2: Signal Handling, More Parsing, and Subshells
- 2.1: Ctrl-C
- 2.2: Zombie Elimination
- 2.3: Exit
- 2.4: Quotes
- 2.5: Escaping
- 2.6: Builtin Functions (printenv, setenv A B, unsetenv A, source A, cd A)
- 2.7: Creating a Default Source File: “.shellrc”
- 2.8: Subshells

Part 3: Expansions, Wildcards, and Line Editing (in progress)
- 3.1: Environment variable expansion "${$}, ${?}, ${!}, ${_}, ${SHELL}"
- 3.2: Tilde expansion
- 3.3: Wildcarding
- 3.4: Edit mode
- 3.5: History
- 3.6: Path completion
- 3.7: Variable prompt




