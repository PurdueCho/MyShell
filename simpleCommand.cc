#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <iostream>

#include "simpleCommand.hh"

SimpleCommand::SimpleCommand() {
  _arguments = std::vector<std::string *>();
}

SimpleCommand::~SimpleCommand() {
  // iterate over all the arguments and delete them
  for (auto & arg : _arguments) {
    delete arg;
  }
}

void SimpleCommand::insertArgument( std::string * argument ) {
  // simply add the argument to the vector
	
/*
	char *cstr = new char[argument->length() + 1];
	strcpy(cstr, argument->c_str());
	
	int a = 0;
	int b = 0;
	int c = 0;

	char *nstr = new char[argument->length()*2];

	while(cstr[a] != '\0'){
		if(cstr[a] == '\\' && cstr[a+1] == '$'){
			a++;
			while(cstr[a] != '}'){
				nstr[b++] = cstr[a++];  // exract the arguments inside of ${}
			}
		}
		else if (cstr[a] == '$' && argument[a+1] == '{'){
			a+=2;
			while( cstr[a] != '}' ){
				globVar[counter] = argument[oldArgCounter];
				counter+=1;
				oldArgCounter+=1;
			}
			oldArgCounter+=1;
			globVar[counter] = '\0';
			globVarVal = getenv(globVar);	
			for(int i = 0; i < strlen(globVarVal); i++){
				newArgument[newArgCounter] = globVarVal[i];
				newArgCounter++;
			}
		}else{
			newArgument[newArgCounter] = argument[oldArgCounter];
			newArgCounter+=1;
			oldArgCounter+=1;
			
		}
	}
	newArgument[newArgCounter] = '\0'; 

	_arguments[ _numberOfArguments ] = newArgument;
	
	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
	
	
	//delete [] cstr;
*/
	  _arguments.push_back(argument);
}

// Print out the simple command
void SimpleCommand::print() {
  for (auto & arg : _arguments) {
    std::cout << "\"" << *arg << "\" \t";
  }
  // effectively the same as printf("\n\n");
  std::cout << std::endl;
}
