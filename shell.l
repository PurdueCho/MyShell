
/*
 *
 * CS-252 
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <cstring>
#include <iostream>
#include "y.tab.hh"

static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

%}

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

"<" {
	return LESS;
}

">>" {
	return GREATGREAT;
}

"&" {
	return AMPERSAND;
}

">&" {
	return GREATAMPERSAND;
}

">>&" {
	return GREATGREATAMPERSAND;
}

"|" {
	return PIPE;
}

"2>" {
	return INTEGERGREAT;
}

\$\([^\n\$]*\)|`[^\n`]*` {
	/* Generate subshell */
	int i=0;

	std::string * cmd = new std::string(yytext);
	cmd->pop_back();

	// string check
	if (strchr(cmd->c_str(), '`') != NULL) { // if `...`
		cmd = new std::string(yytext+1);
		cmd->pop_back();
	}
	else {
		cmd = new std::string(yytext+2);  // if $(...)
		cmd->pop_back();
	}

	// Setup i/op redirection in subshell
	// and call exec
	int tmpin = dup(0);
	int tmpout = dup(1);

	// Parent creates two pipes
	int fdpipein[2];
	int fdpipeout[2];

	pipe(fdpipein);
	pipe(fdpipeout);

	// write subshell command 
	write(fdpipein[1], cmd->c_str(), cmd->size());
	write(fdpipein[1], "\n", 1);
	write(fdpipein[1], "exit", 4);
	write(fdpipein[1], "\n", 1);
	close(fdpipein[1]);

	dup2(fdpipein[0], 0);
	close(fdpipein[0]);
	dup2(fdpipeout[1], 1);
	close(fdpipeout[1]);

	// In child redirect inout from pin[0] and redirect
	// output to pout[1]. Then execute shell from
	// /proc/self/exe
	int ret = fork();
	if (ret == 0) {
		execvp("/proc/self/exe", NULL);
		_exit(1);
	} else if (ret < 0) {
		perror("fork");
		exit(1);
	}

	// restore in/out defaults
	dup2(tmpin, 0);
	dup2(tmpout, 1);
	close(tmpin);
	close(tmpout);

	// read from fdpipeout
	char ch;
	size_t size = 0;
	i = 0;

	// The parent reads the ouput from pout[0] and
	// writes it into a buffer.
	char * buffer = new char[6666];
	while (read(fdpipeout[0], &ch, 1)) {
		if ( ch == '\n' ? buffer[i++] = ' ' : buffer[i++] = ch);
	}
	buffer[i] = '\0';

	// The parent puts the characters
	// in the buffer back into lex by calling myunoutc(in c)
	// in reverse order.
	i = strlen(buffer) - 1; // Piazza student's answer helped it. Errors occurs when it begins with '\0'
	for (i ; i >= 0; i--) {
		unput(buffer[i]);
	}
/*
	yylval.cpp_string = new std::string(buffer);
	return WORD;
*/
}



[^ \t\n|><&]*\\[^ \t\n]* {
	
	/* Allow the escape character. */
	
	int i=0;
	char * escChar;
	escChar = new char[strlen(yytext)*2];
	char * args = yytext;
	while ( *args ) {
		if ( *args == '\\' ) {	
			escChar[i] = *(++args);
			i++;	
		} else {
			escChar[i] = *args;
			i++;
		}
		args++;
	}
	escChar[i] = '\0';
	yylval.cpp_string = new std::string(escChar);
	return WORD;
	
}

["][^\n\"]*["] {
	/* Add support for quotes in your shell. */
	int i = 0;
	char * quoteChar = new char[strlen(yytext)];
	char * args = yytext;
	
	while ( *args ) {
		if ( *args == '\"' ) {
			quoteChar[i] = *(++args);
			i++;
		} else {
			quoteChar[i] = *args;
			i++;
		}
		args++;
	}
	quoteChar[i] = '\0';
	yylval.cpp_string = new std::string(quoteChar);
	return WORD;
}

[^ \t\n|><][^ \t\n|><]* {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}






