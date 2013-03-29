/* sis.c: Simple shell main function
 * by Josh Armstrong
 * for CS240, Axel Krings
 */

#include <stdio.h>	//printf
#include <stdlib.h>	//fork, execvp, waitpid
#include <string.h>	//strcmp
#include "command.h" 	//makearg, getCommand
#include "runCustom.h"  //runCustom, run...

int main() {
  char *command;	//the user's command
  NodePtr aliaslisthead;//the head of the linked list of aliases
  int aliascount;	//the number of aliases
  char *path;

  aliascount = 0;
  aliaslisthead = NULL;

  //execute the .sisrc file
  //NYI

  //print out a prompt and get the first command
  printf("\%: ");	
  command = getCommand(); 

  //loop while the user doesn't type exit
  while (strcmp(command, "exit") != 0) {
    //process the command entered
    runCustom(command, &aliaslisthead, &aliascount);
 
    //print out prompt and get another command
    printf("\%: ");
    command = getCommand();
  }

  return 0;
}

