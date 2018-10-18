#include <cstdio>
#include <unistd.h>
#include "shell.hh"
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int yyparse(void);
void yyrestart(FILE * file);
void Shell::prompt() {
  //printf("myshell>");
  fflush(stdout);
}

extern "C" void sig_handler_ctrlC ( int sig ) {
	if (sig == SIGINT) {
		printf("\nEXITING PROGRAM\n");
		exit(EXIT_SUCCESS);
	}
}

extern "C" void sig_handler_zombie ( int sig ) {
	pid_t pid = wait3(0, 0, NULL);
	while (waitpid(-1, NULL, WNOHANG) > 0) {
		printf("\n[%d] exited.", pid);
	}
	
}

int main() {
	// Ctrl - C //
	struct sigaction sig_int_action;
	sig_int_action.sa_handler = sig_handler_ctrlC;
	sigemptyset(&sig_int_action.sa_mask);
	sig_int_action.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &sig_int_action, NULL)) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	// Ctrl - C END// 

	// Zombie end //
	struct sigaction sig_int_action2;
	sig_int_action2.sa_handler = sig_handler_zombie;
	sigemptyset(&sig_int_action2.sa_mask);
	sig_int_action2.sa_flags = SA_RESTART;

	if (sigaction(SIGCHLD, &sig_int_action2, NULL)) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	// Zombie end //

	FILE * fd = fopen(".shellrc", "r");
	if (fd) {
		yyrestart(fd);
		yyparse();
		yyrestart(stdin);
		fclose(fd);
	} else {
		if ( isatty(0) ) {
  			Shell::prompt();
		}
	}
/*
	char s[20];
	fgets(s, 20, stdin);
	if ( !strcmp( s, "exit\n" ) ) {
		printf( "Bye!\n");
		exit(1);
	}
*/
	  yyparse();
}

Command Shell::_currentCommand;
