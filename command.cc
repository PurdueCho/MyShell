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

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "command.hh"
#include "shell.hh"


Command::Command() {
	// Initialize a new vector of Simple Commands
	_simpleCommands = std::vector<SimpleCommand *>();

	_outFile = NULL;
	_inFile = NULL;
	_errFile = NULL;
	_background = false;
	_append = false;
	_count=0;
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

	if ( _outFile ) {
		delete _outFile;
	}
	_outFile = NULL;

	if ( _inFile ) {
		delete _inFile;
	}
	_inFile = NULL;

	if ( _errFile ) {
		delete _errFile;
	}
	_errFile = NULL;

	_background = false;
	_append = false;
	_count = 0;
}

void Command::print() {
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");

	int i = 0;
	// iterate over the simple commands and print them nicely
	for ( auto & simpleCommand : _simpleCommands ) {
		printf("  %-3d ", i++ );
		simpleCommand->print();
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n",
			_outFile?_outFile->c_str():"default",
			_inFile?_inFile->c_str():"default",
			_errFile?_errFile->c_str():"default",
			_background?"YES":"NO");
	printf( "\n\n" );
}

extern char **environ;

void Command::execute() {
	//print();
	// Don't do anything if there are no simple commands
	if ( _simpleCommands.size() == 0 ) {
		Shell::prompt();
		return;
	}

	// When IO modifier is ambiguous
	// give error message
	if (_count > 1) {
		printf("Ambiguous output redirect.\n");
		clear();
		Shell::prompt();
		return;
	}

	// ByeBye
	if ( !strcmp(_simpleCommands[0]->_arguments[0]->c_str(),"exit")) {
		printf( "Bye!\n" ); 
		_exit(1);
	}



	// Print contents of Command data structure
	//print();

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec

	// save in/out
	int tmpin=dup(0);
	int tmpout=dup(1);
	int tmperr=dup(2);

	// set the initial file directory
	int fdin;
	int fdout;
	int fderr;

	// _errFile 
	if(_errFile){
		if(_append){ // if append is true
			fderr = open(_errFile->c_str(), O_WRONLY | O_APPEND | O_CREAT, 0655);
		}
		else {
			fderr = open(_errFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0655);
		}
	}
	else {
		fderr = dup(tmperr); // default
	}
	dup2(fderr,2);
	close(fderr);

	// _inFile
	if (_inFile) {
		fdin = open(_inFile->c_str(), O_RDONLY);
	}
	else {
		// Use default input
		fdin = dup(tmpin);
	}


	int ret;
	int numberOfSimpleCommands = _simpleCommands.size(); 


	for (int i = 0; i < numberOfSimpleCommands ; i++) {
		//redirect input
		dup2(fdin, 0);
		close (fdin);

		/********** Built - in command START **********/
		if(!strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "setenv")){
			if(setenv(_simpleCommands[i]->_arguments[1]->c_str(), _simpleCommands[i]->_arguments[2]->c_str(), 1)) {
				perror("setenv");
			}
			clear();
			Shell::prompt();
			return;
		} 
		if(!strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "unsetenv")){

			if(unsetenv(_simpleCommands[i]->_arguments[1]->c_str())) {
				perror("unsetenv");
			}
			clear();
			Shell::prompt();
			return;
		}
		if (!strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "cd")) {
			if (_simpleCommands[i]->_arguments.size() == 1) {
				if( chdir(getenv("HOME"))) perror("cd");
			} else {
				const char * path = _simpleCommands[i]->_arguments[1]->c_str();
				char message[1024] = "cd: can't cd to ";
				strcat(message, path);	
				if (chdir(path)) perror(message);
			}
			clear();
			Shell::prompt();
			return;
		}
		if (!strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "source")) {
				// Read source from FILE

				std::string cmd;
				std::ifstream fd;

				fd.open(_simpleCommands[i]->_arguments[1]->c_str());

				std::getline(fd, cmd);
				fd.close();

				// save in/out
				int tmpin=dup(0);
				int tmpout=dup(1);

				// input command
				int fdpipein[2];
				pipe(fdpipein);
				write(fdpipein[1], cmd.c_str(), strlen(cmd.c_str()));
				write(fdpipein[1], "\n", 1);

				close(fdpipein[1]);

				int fdpipeout[2];
				pipe(fdpipeout);

				dup2(fdpipein[0], 0);
				close(fdpipein[0]);
				dup2(fdpipeout[1], 1);
				close(fdpipeout[1]);

				int ret_source = fork();
				if (ret_source == 0) {
					execvp("/proc/self/exe", NULL);
					_exit(1);
				} else if (ret < 0) {
					perror("fork");
					exit(1);
				}

				dup2(tmpin, 0);
				dup2(tmpout, 1);
				close(tmpin);
				close(tmpout);	

				char ch;
				char * buffer = new char[i];
				int r = 0;

				// read output 
				while (read(fdpipeout[0], &ch, 1)) {
					//if (ch == '\n' ? buffer[i++] = '\n' : buffer[i++] = ch) {};
					if (ch != '\n')  buffer[r++] = ch;
				}

				buffer[r] = '\0';
				printf("%s\n",buffer);

				fflush(stdout);

				clear();
				Shell::prompt();
				return;
			}
		/********** Built - in command END ***********/

		//setup output
		if ( i == numberOfSimpleCommands -1 ) {
			// last simple command
			if (_outFile) {
				if (_append) {
					fdout = open(_outFile->c_str(), O_WRONLY | O_APPEND | O_CREAT, 0655);
				} else {
					fdout=open(_outFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0655);
				}
			}
			else {
				//Use default output
				fdout=dup(tmpout);
			}
		}
		else {
			// Not last
			// simple command
			// create PIPE
			int fdpipe[2];
			pipe(fdpipe);
			fdout=fdpipe[1];
			fdin=fdpipe[0];
		}	

		// redirect output
		dup2(fdout, 1);
		close(fdout);

		// create child
		ret = fork();

		// converting vector to char **
		size_t _simpleArgumentsNum = _simpleCommands[i]->_arguments.size();
		char ** _args = new char*[_simpleArgumentsNum+1];
		size_t j;
		for (j = 0; j < _simpleArgumentsNum ; j++) {
			_args[j] = strdup(_simpleCommands[i]->_arguments[j]->c_str());
		}
		_args[j]=NULL;
		//

		if (ret == 0) { // child
			/*********** BUILT -in command Start *********/
			if (!strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "printenv")) {
				char ** p = environ;

				while(*p != NULL){
					printf("%s\n", *p);
					p++;
				}
				fflush(stdout);
				_exit(1);
			}
			/************ BUILT - in command END ***********/

			execvp(_simpleCommands[i]->_arguments[0]->c_str(), _args);
			perror("execvp");
			_exit(1);

		} else if (ret < 0) {
			perror( "fork");
			return;
		}

	}// for

	// restore in/out defaults
	dup2(tmpin,0);
	dup2(tmpout,1);
	dup2(tmperr,2);
	close(tmpin);
	close(tmpout);
	close(tmperr);

	if (!_background) {
		// Wait for last command
		waitpid(ret, NULL, 0);
	}



	// Clear to prepare for next command
	clear();

	// Print new prompt
	if ( isatty(0) )
		Shell::prompt();
}



SimpleCommand * Command::_currentSimpleCommand;
