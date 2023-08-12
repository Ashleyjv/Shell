
/*
 *
 * CS-252 
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <cstring>
#include "y.tab.hh"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "shell.hh"
#include <unistd.h>
#include <limits.h>



static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

%}

%option noyywrap

%%

\n {
  return NEWLINE;
}

[ \t] {
  /* Discard spaces and tabs */
}

">" {
  return GREAT;
}



">>" {
  return GREATER;
}

"2>" {
  return TWOGREAT;
}
"|" {
  return PIPE;
}

"&" {
  return AMP;
}

"<" {
  return LESS;
}

">&" {
  return GREATAMP;
}

">>&" {
  return GREATGREATAMP;
}

`[^\n`]*`|\$\([^\n\$]*\) {

  std::string cmd = std::string(yytext);
  std::string removed = cmd.substr(1, cmd.length() - 2);
  std::string fcmd = removed;
  if (removed.at(0) == '(') {
    fcmd = removed.substr(1,removed.length() - 1);
  } 


  int tmpin = dup(0);
  int tmpout = dup(1);
  int pin[2];
  int pout[2];
  pipe(pin);
  pipe(pout);
  char *com = const_cast<char*> (fcmd.c_str());
  write(pin[1], com, strlen(com));
  write(pin[1], "\n", 1);
  write(pin[1], "exit\n", 5);
  close(pin[1]);

  dup2(pin[0], 0);
  close(pin[0]);
  dup2(pout[1], 1);
  close(pout[1]);

  int retval;
  retval = fork();
  if (!retval) {
    execvp("/proc/self/exe", NULL);
    perror("execvp");
    exit(1);
  }
  // else if (retval < 0) {
  //   perror("fork");
  //   exit(1);
  // }
  dup2(tmpin, 0);
  dup2(tmpout, 1);
  close(tmpin);
  close(tmpout);
  int counter = 0;
  char buffer[10000];
  char each = '\n';
  while (read(pout[0], &each, 1)) {
    if (each == '\n') {
      buffer[counter] = ' ';
    }
    else {
      buffer[counter] = each;
    } counter++;
  }
  for (int j = counter - 12; j >= 0; j--) {
    myunputc(buffer[j]);
  }
}

"source "[^ \t\n]+ {
  std::string name = std::string(yytext + 7);
  char buffer[10000];
  FILE *ffile = fopen(name.c_str(), "r");
  int counter = 0;
  if (ffile) {
    while (fscanf(ffile, "%c", buffer+counter) != EOF) {
      counter = counter + 1;
    }
  }
  fclose(ffile);
  ffile = NULL;
  for (int j = counter - 1; j >= 0; j--) {
    myunputc(buffer[j]);
  }
}


"source "[^ \t\n]+ {
  std::string name = std::string(yytext + 7);
  char charac[10000];
  FILE *ffile = fopen(name.c_str(), "r");
  int counter = 0;
  if (ffile) {
    while (fscanf(ffile, "%c", charac+counter) != EOF) {
      counter = counter + 1;
    }
  }
  fclose(ffile);
  ffile = NULL;
  for (int j = counter - 1; j >= 0; j--) {
    myunputc(charac[j]);
  }
}

["][^\t\n\"]*["] {
  std::string mystring = std::string(yytext);
  mystring = mystring.substr(1,mystring.length() - 2);
  yylval.cpp_string = new std::string(mystring);
  return WORD;
} /* dont know if this works */ /*QUOTe */

[^ \t\n]*\\[^ \t\n]* {
  int j = 0;
  char * cmd = yytext;
  char *final = new char[strlen(yytext)];
	while (*cmd) {
		if (*cmd == '\\') {	
      cmd++;
		  final[j] = *(cmd);
			j++;	
		} else {
			final[j] = *cmd;
			j++;
		}
		cmd++;
	}
	final[j] = '\0';
	yylval.cpp_string = new std::string(final);
	return WORD;
}

[^ \t\n|<>][^ \t\n|<>]*  {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}









