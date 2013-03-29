#ifndef VARIABLE_H
#define VARIABLE_H

#include "command.h"
#include <stdio.h>

int processVar(char **args, NodePtr *varlisthead);
char* evalString(char* string, NodePtr *varlisthead);
NodePtr findVar(char *varname, NodePtr varlisthead);
int tokenizeVars(char* string, char ***args);

int processVar(char **args, NodePtr *varlisthead) {
  NodePtr variable;

  //find out which variable we are setting
  //set that to whatever args[1] is. evaulate args[1].
  variable = findVar(args[0], *varlisthead);
  variable->value2 = evalString(args[1], varlisthead);
}

char* evalString(char* string, NodePtr *varlisthead) {
  //this function needs a function that can tokenize a string based on a 
  //set of characters, and include the characters in the token list. 
  //OR, you could find the first occurence of the a symbol (except$), then
  //start from there on the token. 
  //once a list of tokens is available, replace the valid $vars with values
  //and return.
}

int tokenizeVars(char* string, char ***args) {
  /* 1. find first occurence of valid symbol. 
   * 2. token from start to first valid symbol.
   * 3. search again for valid symbol.
   * 4. token from last valid symbol to this valid symbol.
   * 5. return list of token, symbol, token, symbol.
   */

  char *newtoken;
  char *tokenend;
  char *tokenstart;
  NodePtr newnode, tokenhead;
  int tokencount;

  tokenstart = string;
  tokenhead = NULL;

  while ((tokenend = strpbrk(string, "~@#%^*+{}[]:,./")) != NULL) {
    printf("found new token at index %i\n", tokenend-tokenstart);
    newtoken = malloc((int)(tokenend - tokenstart) * sizeof(char));
    strncpy(newtoken, string, (tokenend - tokenstart));
    
    /*newnode = malloc(sizeof(Node));
    if (newnode == NULL) { return -1; }
    newnode->token = newtoken;
    newnode->next = tokenhead;
    tokenhead = newnode;*/
    
    printf("token: (%s)\n", newtoken);
    free(newtoken);
 
    tokencount++; 
    string = tokenend+1; 
    tokenstart = string;
  }

  if (string[0] != '\0') {
    newtoken = malloc((strlen(string)+1)*sizeof(char));
    strncpy(newtoken, string, strlen(string)+1);
    printf("token: (%s)\n", newtoken);
    tokencount++;
    free(newtoken);
  } 

  printf("Found %i tokens.\n", tokencount++);

   
 
  return 0;
}

NodePtr findVar(char *varname, NodePtr varlisthead) {
  NodePtr tempnode;

  tempnode = varlisthead;
  while (tempnode != NULL) {
    if (strcmp(tempnode->value1, varname) == 0) { break; }
  } 

  return tempnode;
}

#endif
