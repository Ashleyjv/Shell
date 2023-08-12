/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT 
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE 
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM  
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */


#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/types.h>
#include "command.hh"
#include "shell.hh"


Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
    appendflag = false;
    
    multiplefiles = false;
}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
    // add the simple command to the vector
    _simpleCommands.push_back(simpleCommand);
}

void Command::clear() {
    // deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommands) {
        delete simpleCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommands.clear();

    // if ( _outFile ) {
    //     delete _outFile;
    // }
    

    if ( _inFile ) {
        delete _inFile;
    }
    _inFile = NULL;

    if ( _errFile == _outFile) {
        delete _errFile;
    }
    else { 
        if (_errFile) {
        delete _errFile;
        }
        if (_outFile){

        
        delete _outFile;
        }
    }
    _outFile = NULL;
    _errFile = NULL;

    _background = false;

    appendflag = false;
    multiplefiles = false;
}

void Command::print() {
    // printf("\n\n");
    // printf("              COMMAND TABLE                \n");
    // printf("\n");
    // printf("  #   Simple Commands\n");
    // printf("  --- ----------------------------------------------------------\n");

    // int i = 0;
    // iterate over the simple commands and print them nicely
    // for ( auto & simpleCommand : _simpleCommands ) {
        // printf("  %-3d ", i++ );
        // simpleCommand->print();
    // }

    // printf( "\n\n" );
    // printf( "  Output       Input        Error        Background\n" );
    // printf( "  ------------ ------------ ------------ ------------\n" );
    // printf( "  %-12s %-12s %-12s %-12s\n",
    //         _outFile?_outFile->c_str():"default",
    //         _inFile?_inFile->c_str():"default",
    //         _errFile?_errFile->c_str():"default",
    //         _background?"YES":"NO");
    // printf( "\n\n" );
}

void Command::execute() {
    // Don't do anything if there are no simple commands
    if ( _simpleCommands.size() == 0 ) {
        Shell::prompt();
        return;
    }
    if (strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "exit") == 0) {
        printf("Good bye!\n");
        exit(0);
    }
    if (multiplefiles) {
        printf("Ambiguous output redirect.\n");
        multiplefiles = false;
        clear();
        Shell::prompt();
        return;
    }
    

    // Print contents of Command data structure
    // print();

    int tmpin = dup(0);
    int tmpout = dup(1);
    int tmperr = dup(2);
    int fdin;
    int fdout;
    int fderr;
    int retval;

    if (_inFile) {
        fdin = open(_inFile->c_str(), O_RDONLY);
    }
    else {
        fdin = dup(tmpin);
    }
    if (_errFile) {
        if (appendflag == true) {
            fderr = open(_errFile->c_str(), O_CREAT| O_WRONLY| O_APPEND, 0664);
        }
        else {
            fderr = open(_errFile->c_str(), O_CREAT|O_WRONLY|O_TRUNC, 0664);
        }
    }
    else {
        fderr = dup(tmperr);
    }
    dup2(fderr, 2);
    close(fderr);
    //execution loop-

    for (int i = 0; i < (int) _simpleCommands.size(); i++) {
        
        if (!strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "setenv")) {
            setenv(_simpleCommands[i]->_arguments[1]->c_str(), 
            _simpleCommands[i]->_arguments[2]->c_str(), 1);
            clear();
            Shell::prompt();
            dup2(fdin, 0);
            close(fdin);
            close(tmperr);
            close(tmpout);
            close(tmpin);
            return;
        }
        else if (!strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "unsetenv")) {
            unsetenv(_simpleCommands[i]->_arguments[1]->c_str());
            clear();
            Shell::prompt();
            dup2(fdin, 0);
            close(fdin);
            close(tmpin);
            close(tmpout);
            close(tmperr);
            return;
        }
        else if (!strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "cd")) {
            if (_simpleCommands[i]->_arguments.size() == 1) {
                chdir(getenv("HOME"));
            }
            else {
                int value = chdir(_simpleCommands[i]->_arguments[1]->c_str());
                    if (value ==-1) {
                        fprintf(stderr, "cd: can't cd to %s\n", _simpleCommands[i]->_arguments[1]->c_str());
                    }
                }
            clear();
            Shell::prompt();
            dup2(fdin, 0);
            close(fdin);
            close(tmpin);
            close(tmpout);
            close(tmperr);
            return;
            }
            
        dup2(fdin, 0);
        close(fdin);
        if (i == (int) _simpleCommands.size() -1) {
            if (_outFile) {
                if (appendflag) {
                    fdout = open(_outFile->c_str(), O_CREAT| O_WRONLY| O_APPEND, 0664);
                }
                else {
                    fdout = open(_outFile->c_str(), O_CREAT|O_WRONLY|O_TRUNC, 0664);
                }
                if (fdout < 0){
                    perror("open");
                    exit(1);
                }    
            }
            else {
                fdout = dup(tmpout);
            }
        }
        //environment stuff
        //setenv
        
        // // unset
        

        else {
            int fdpipe[2];
            pipe(fdpipe);
            fdout = fdpipe[1];
            fdin = fdpipe[0];
        }
        dup2(fdout, 1);
        close(fdout);
    
        retval = fork();
        if (retval == 0) {
            if(strcmp(_simpleCommands[i]->_arguments[0]->c_str(),"printenv") == 0){
                char **contr = environ;
                while (*contr) {
                    printf("%s\n", *contr);
                    contr++;
                }
                exit(0);
	        }
            else {   
            auto &simpleCommand = _simpleCommands[i];
                int size = simpleCommand->_arguments.size();
                char *argv[size + 1];
                int v = 0;
                for (auto & arg : simpleCommand->_arguments) {
                    argv[v++] = const_cast<char*> ( (*arg).c_str() );
                }
                argv[size] = NULL;
                execvp(argv[0], argv);            
                perror("execvp");
                exit(1);
            }    
        }
    }
    dup2(tmpin, 0);
    dup2(tmpout, 1);
    dup2(tmperr, 2);
    close(tmpin);
    close(tmpout);
    close(tmperr);
    if (!_background) {
        waitpid(retval, NULL, 0);
    }
    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec

    // Clear to prepare for next command ls | grep cc
    clear();

    // Print new prompt
    Shell::prompt();
}

SimpleCommand * Command::_currentSimpleCommand;
