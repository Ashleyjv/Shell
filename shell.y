
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
%token NOTOKEN GREAT NEWLINE GREATER TWOGREAT PIPE AMP LESS GREATAMP GREATGREATAMP

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

//edited
command:
  simple_command;

//what does this do
simple_command:	
  pipe_list iomodifier_list _background  NEWLINE {
    // printf("   Yacc: Execute command\n");
    Shell::_currentCommand.execute();
  }
  | NEWLINE { Shell::prompt(); }
  | error NEWLINE { yyerrok; }
  ;
//new 
pipe_list:
  pipe_list PIPE command_and_args
  | command_and_args
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
    // printf("   Yacc: insert argument \"%s\"\n", $1->c_str());
    Command::_currentSimpleCommand->insertArgument( $1 );\
  }
  ;

command_word:
  WORD {
    // printf("   Yacc: insert command \"%s\"\n", $1->c_str());
    Command::_currentSimpleCommand = new SimpleCommand();
    Command::_currentSimpleCommand->insertArgument( $1 );
  }
  ;
//new
iomodifier_list:
  iomodifier_list iomodifier_opt
  | iomodifier_opt
  | /* empty*/
  ;

iomodifier_opt:
  GREAT WORD {
    if (Shell::_currentCommand._outFile) {
      Shell::_currentCommand.multiplefiles = true;
    }
    Shell::_currentCommand._outFile = $2;
  }
  | GREATER WORD {
    if (Shell::_currentCommand._outFile) {
      Shell::_currentCommand.multiplefiles = true;
    }
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand.appendflag = true;
    //append not happening
  } 
  | GREATGREATAMP WORD {
    if (Shell::_currentCommand._outFile || Shell::_currentCommand._errFile ) {
      Shell::_currentCommand.multiplefiles = true;
    }
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._errFile = $2;
    Shell::_currentCommand.appendflag = true;
  }
  | GREATAMP WORD {
    if (Shell::_currentCommand._outFile || Shell::_currentCommand._errFile ) {
      Shell::_currentCommand.multiplefiles = true;
    }
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._errFile = $2;
  }
  | LESS WORD {
    if (Shell::_currentCommand._inFile) {
      Shell::_currentCommand.multiplefiles = true;
    }
    Shell::_currentCommand._inFile = $2;
  }
  | TWOGREAT WORD {
    if (Shell::_currentCommand._errFile) {
      Shell::_currentCommand.multiplefiles = true;
    }
    Shell::_currentCommand._errFile = $2;
  }
  ;
_background:
  AMP {
      Shell::_currentCommand._background = true;
  }
  | /* empty */
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
