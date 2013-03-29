#ifndef RUNCUSTOM_H
#define RUNCUSTOM_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>	//fork, execvp, waitpid
#include <string.h>
#include "command.h"
#include "variable.h"

int runCustom(char *command, NodePtr *aliaslisthead, int *aliascount);


/* runCustom: executes a command, processing shell and system commands
 * char *command: the command to execute
 * NodePtr *aliaslisthead: pointer to the head of the linked list of aliases
 * int *aliascount: pointer to the aliascount
 * NOTE: this doesn't handle setting the PATH variable or history commands yet 
 */
int runCustom(char *command, NodePtr *aliaslisthead, int *aliascount) {
  /* 1. check if the command is an alias
   * 2. if it is, run the alias command with runCustom
   * 3. if this is an alias command, process that with processAlias
   * -. Check if this is setting a new path. if it is, run processVar
   * -. Check if this is a history command, e.g. it starts with !. 
        Run processHistory
   * 4. otherwise, run the system command
   * 5. return the result of either the alias or the system command
  */

  char *newcommand;	//the new command after an alias has been replaced
  char **args;		//the tokenized command
  int argcount;		//argument count

  //make some arguments
  argcount = makearg(command, &args, ' ');
  
  /* 1. check if the command is an alias */
  if (argcount == 1) {  //this needs to be changed to a function argument
    NodePtr tempnode;
    tempnode = (*aliaslisthead);
    while (tempnode != NULL) {
      if (strcmp(tempnode->value1, args[0]) == 0) {
        /* found match. expand the alias and rerun the command. if
         * the alias ends with a space or tab check the next argument for an
         * alias. */
        /* replace this argument with alias. put arguments together. run
         * the resulting command again. */
        args[0] = tempnode->value2;
        if (endsWithSpaceOrTab(tempnode->value2)) {
          //check next argument (NYI)
          printf("need to check next argument for alias\n");
        }
        newcommand = makeStringFromArguments(args, argcount, ' ');
        return runCustom(newcommand, aliaslisthead, aliascount);
      }
      tempnode = tempnode->next;
    }
  }
  
  /* 2. check if this is an alias command, e.g. the first argument is alias */
  if (strcmp(args[0], "alias") == 0) {
    /* 3. if it is, process with processAlias */
    return processAlias(args, argcount, aliaslisthead, aliascount);
  }

  /* 2. check if this an unalias command, e.g. the first argument is unalias */
  if (strcmp(args[0], "unalias") == 0) {
    return processUnalias(args, argcount, aliaslisthead, aliascount);
  }
 
  /* -. check if this is setting a new path */
  //if there is one argument and that argument has two args when split
  // '=' sign
  //return processVar(args[0]);

  /* -. Check if this is a history command */
  //if (args[0][0] == '!') {
  //  return processHistory(args, argcount, histfilename);
  //}

  /* 4. Otherwise, run the system command */
  return runSystemCommand(args, argcount);

}

/* processUnalias: handles the unalias command
 * char **args: the tokenized command
 * int argc: number of arguments
 * NodePtr *aliaslisthead: pointer to the head of the alias linked list
 * int *aliaslistcount: pointer to the alist count
 * returns -1 for failure and 0 for success
 */
int processUnalias(char **args, int argc, NodePtr *aliaslisthead, int *aliaslistcount) {
  int option;			//the option returned from getopt
  int index;			//the index into args
  NodePtr tempnode = NULL;	//used for traversing the linked list
  NodePtr nodebefore = NULL;	//used for deleting in the linked list

  optind = 0;	//this resets getopt (optind is a global variable)

  //see the GNU getopt documentation for more information, this is 
  //basically taken from the example
  while ((option = getopt(argc, args, "a")) != -1) {
    switch (option) { 
      case 'a':
        //remove all aliases
        tempnode = (*aliaslisthead);
        while (tempnode != NULL) {
          nodebefore = tempnode;
          tempnode = tempnode->next;
          free(nodebefore);
        }
        (*aliaslistcount) = 0;
        break;
      case '?':
        printf("-sis: unalias: -%c: invalid option\nunalias: usage: unalias [-a] [name ... ]", optopt);
        return -1;
      default:
        abort();
    }
  }  

  //process the non-options, e.g. the aliases to remove
  for (index = optind; index < argc; index++) {
    tempnode = (*aliaslisthead);
    while (tempnode != NULL) {
      if (strcmp(tempnode->value1, args[index]) == 0) {
        //remove this one
        if (nodebefore == NULL) {	//remove the list head
          (*aliaslisthead) = tempnode->next;
          free(tempnode);
        } else {			//remove in the middle of the list
          nodebefore->next = tempnode->next;
          free(tempnode);
        }
        (*aliaslistcount)--;
        break;
      }
      nodebefore = tempnode;
      tempnode = tempnode->next;
    }
    if (tempnode == NULL) {
      printf("-sis: unalias: %s: not found\n", args[index]);
    }
  }
}

/* processAlias: handles the alias command, creating or displaying aliases
 * char ** args: the list of arguments, with "alias" the first argument
 * int argc: the number of arguments, including "alias"
 * Node *aliaslisthead: a pointer the head of the alias list (modifiable)
 * int *aliascount: a pointer the alias count, modifiable
 * Returns -1 for failure, 0 to success
 */
int processAlias(char **args, int argc, NodePtr *aliaslisthead, int *aliascount) {
  int option;		//the char returned from getopt
  int index; 		//the index int the args that are non-options
  char **nameargs;	//the arguments from the 'name=value' with '=' delim
  int nameargcount;  	//the number of arguments in nameargs (should be 1 or 2)
  NodePtr tempnode; 	//used for looping over list and creating new nodes

  //setting the global optind variable to 0 forces getopt to reset
  optind = 0;

  while ((option = getopt(argc, args, "p")) != -1) {
    switch (option) {
      case 'p':
        tempnode = (*aliaslisthead);
        while (tempnode != NULL) {
          printf("alias %s='%s'\n", tempnode->value1, tempnode->value2);
          tempnode = tempnode->next;
        } 
        break;
      case '?':
        printf("-sis: alias: -%c: invalid option\nalias: usage: alias [-p] [name[=value] ... ]", optopt);
        return -1;
      default:
        printf("default\n"); 
     }
  }

  //process the 
  for (index = optind; index < argc; index++) {
    //argument should be in the format [name[=alias]]
    nameargcount = makearg(args[index], &nameargs, '=');
    if (nameargcount == 1) {
      //print out the value for this alias
      tempnode = (*aliaslisthead);
      while (tempnode != NULL) {
        if (strcmp(tempnode->value1, nameargs[0]) == 0) {
           printf("alias %s='%s'\n", tempnode->value1, tempnode->value2);
           break;
        }
        tempnode = tempnode->next;
      }
      if (tempnode == NULL) {
        printf("-sis: alias: %s: not found\n", nameargs[0]);
      }
    } else if (nameargcount == 2) {
      //check if the alias has already been created
      tempnode = (*aliaslisthead);
      while (tempnode != NULL) {
        if (strcmp(tempnode->value1, nameargs[0]) == 0) {
          //redefine this alias
          tempnode->value2 = removeQuotations(&nameargs[1]);
          break;
        }
      } 
      if (tempnode == NULL) {
        //create a new alias
        tempnode = malloc(sizeof(Node));
        if (tempnode == NULL) { return -1; }
        tempnode->value1 = nameargs[0];
        tempnode->value2 = removeQuotations(&nameargs[1]);
        tempnode->next = (*aliaslisthead);
        (*aliaslisthead) = tempnode;
        (*aliascount)++;
      }
    }
  }
 
  return 0;
}

int runSystemCommand(char **args, int argcount) {
  int returnvalue;
  int childid;		//the child's process id
  int execresult;	//the result of the exec function
  returnvalue = 0;

  //fork the program
  childid = fork();
  if (childid == 0) {
    //if we are now the child, execute the command
    execresult = execvp(args[0], args);
    if (execresult == -1) {
      //if the exec failed then exit this child program
      printf("-sis: %s: command not found\n", makeStringFromArguments(args, argcount, ' '));
      exit(-1);
    }
  } else {
    //if we are the parent then wait for the child to terminate
    waitpid(childid, &execresult, 0);
    returnvalue = execresult;
  }

  return returnvalue;
}
#endif
