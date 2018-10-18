
/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%code requires 
{
#include <string>

#if __cplusplus > 199711L
#define register      // Deprecated in C++11 so remove the keyword
#endif
}

%union
{
  char        *string_val;
  // Example of using a c++ type in yacc
  std::string *cpp_string;
}

%token <cpp_string> WORD
%token NOTOKEN GREAT NEWLINE LESS GREATGREAT AMPERSAND GREATAMPERSAND GREATGREATAMPERSAND PIPE INTEGERGREAT

%{
//#define yylex yylex
#include <cstdio>
#include "shell.hh"

void yyerror(const char * s);
int yylex();

%}

%%

goal:
  commands
  ;

commands:
  command
  | commands command
  ;

command: simple_command
       ;

simple_command:	
	pipe_list iomodifier_list background_optional NEWLINE {
    Shell::_currentCommand.execute();
  }
  | NEWLINE { Shell::prompt();} 
  | error NEWLINE { yyerrok; }
  ;

command_and_args:
  command_word argument_list {
    Shell::_currentCommand.
    insertSimpleCommand( Command::_currentSimpleCommand );
  }
  ;

argument_list:
  argument_list argument
  | /* can be empty */
  ;

argument:
  WORD {
    Command::_currentSimpleCommand->insertArgument( $1 );
  }
  ;

command_word:
  WORD {
    Command::_currentSimpleCommand = new SimpleCommand();
    Command::_currentSimpleCommand->insertArgument( $1 );
  }
  ;

pipe_list:
	pipe_list PIPE command_and_args
  	| command_and_args
  	;


iomodifier_opt:
  GREAT WORD {
    Shell::_currentCommand._outFile = $2;
	Shell::_currentCommand._count++;
  }
  | LESS WORD {
	Shell::_currentCommand._inFile = $2;
	Shell::_currentCommand._count++;

  }
  | GREATGREAT WORD {
	Shell::_currentCommand._outFile = $2;
	Shell::_currentCommand._append = true;
	Shell::_currentCommand._count++;

  }
  | GREATAMPERSAND WORD {
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._errFile = $2;
	Shell::_currentCommand._count++;

	}
	| GREATGREATAMPERSAND WORD {
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._errFile = $2;
    Shell::_currentCommand._append = true;
	Shell::_currentCommand._count++;

	}
	| INTEGERGREAT WORD {
	Shell::_currentCommand._errFile = $2;
	Shell::_currentCommand._count++;
	}
  ;

iomodifier_list:
	iomodifier_list iomodifier_opt
	|
	;

background_optional:
	AMPERSAND {
		Shell::_currentCommand._background = true;
	}
	|
	;



%%

void
yyerror(const char * s)
{
  fprintf(stderr,"%s", s);
}

#if 0
main()
{
  yyparse();
}
#endif
