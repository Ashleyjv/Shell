#include <cstdio>
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.hh"
#include <string.h>
#include <unistd.h>

int yyparse(void);

void Shell::prompt() {
  if (isatty(0)) {
      printf("myshell>");
  }
  fflush(stdout);
}

extern "C" void cc(int sig) {
  if (sig == SIGINT) {
    fprintf(stdout, "\n");
    Shell::prompt();
  }
}
extern "C" void disp(int sig) {
  if (sig == SIGCHLD) {
      while(waitpid(-1, NULL, WNOHANG) > 0); 
  }
}

int main(int argc, char *argv[]) {
  struct sigaction sa;
    sa.sa_handler = cc;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if(sigaction(SIGINT, &sa, NULL)){
        perror("sigaction");
        exit(2);
    }
    struct sigaction sb;
    sb.sa_handler = disp;
    sigemptyset(&sb.sa_mask);
    sb.sa_flags = 0;

    if(sigaction(SIGCHLD, &sa, NULL)){
        perror("sigchild action");
        exit(2);
    }

  Shell::prompt();
  yyparse();
}

Command Shell::_currentCommand;
