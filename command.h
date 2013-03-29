/* command.h Helper functions for the sis simple shell
 * by Josh Armstrong
 * for CS240, Axel Krings
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>	//getline
#include <stdlib.h> 	//malloc
#include <string.h>     //strncpy, strlen, strcpy, strncat

char* chompByString(char **toParse, char delim);
int makearg(char *s, char ***args, char delim);
void removeNewLines(char **str, int charcount);
char* removeQuotations(char **str);
int endsWithspaceOrTab(char *string);
char* makeStringFromArguments(char **args, int charcount, char delim);

/* struct Node: a node in linked list, with a single token string as data
 * Used when constructing the argument list
 */
typedef struct node Node;
struct node {
  char* token;	//the node data
  char* value1;
  char* value2;
  Node* next;  	//the next node in the list
};
typedef Node* NodePtr;

/* getCommand: returns a single line of user input not including newline. USes
 *             the GNU getline function
 */
char* getCommand() {
  char *str;		//the result string
  size_t str_size = 1;	//the resulting size of the input string

  //Allocate a little bit of memory. getline will reallocate as necessary
  str = (char *) malloc (str_size + 1);
  //use the GNU getline function to safely input a line
  getline(&str, &str_size, stdin);
  //remove the newline that getline returns
  removeNewLines(&str, str_size);

  return str;
}

/* makearg: Returns the number of arguments in s and puts an array of the 
 *          arguments in array pointed to by args or returns -1 for error
 * char *s: The command to find the arguments in
 * char ***args: A pointer to an array of strings that will hold the 
 *               the found arguments
 */
int makearg(char *s, char ***args, char delim) {
  char* token;			//token return by chompByString
  NodePtr tokenhead = NULL;	//head of linked list
  NodePtr newnode = NULL;  	//temp node for adding
  NodePtr deletenode = NULL;	//temp node for deleting
  int argc = 0;			//argument count
  int i;			//loop counter
 
  //search for tokens and add them to the list until the null
  //character is found
  //argc = 1;
  while (s[0] != '\0') {
    //get another token
    token = chompByString(&s, delim);
    
    //if the token returned wasn't a null string (which would mean that 
    //s was just a string of delimiters at this point)
    if (token[0] != '\0') {
      //add on a new node
      newnode = malloc(sizeof(Node));
      if (newnode == NULL) { return -1; }
      newnode->token = token;
      newnode->next = tokenhead;
      tokenhead = newnode;

      argc++;
    } 
  }

  //allocate some memory for the array of strings know that the number
  //of tokens is known
  (*args) = (char**)malloc((argc + 1)* sizeof(char*));

  //iterate through the list and add the tokens to the array
  //start at the tail
  newnode = tokenhead;
  //set i to the number tokens because we are at tail
  i = argc;
  (*args)[i] = NULL;
  i--;
  while (newnode) {
    //set the item at the index
    (*args)[i] = newnode->token;    
    //decrement downwards
    i--;
    //delete the current node and move to the next one
    deletenode = newnode;
    newnode = newnode->next;
    free(deletenode);
  }

  return argc; 
}

/* chompByString: Returns a token deliminated by delim and removes it 
 *                from toParse
 * char **toParse: A pointer to the string to search. This string will be 
 *                 edited by removing the found token and preceding delims
 * char delim: The deliminated character.
 */ 
char* chompByString(char **toParse, char delim) {
  int i;		//index into toParse
  int offset;		//index of first non-delim character
  char* result;		//the resulting token
  int insidequotes;	//boolean for when i is inside quites, i.e. skip delims
  insidequotes = 0;

  //return empty string if toParse is an empty string
  if (((*toParse)[0] == '\0')) {
    *toParse = &(*toParse)[i];
    return *toParse;	
  }

  //Look for first non-delim character while staying inside the string   
  i = 0;
  while (((insidequotes == 0) &&
         (((*toParse)[i] != '\0') &&
          ((*toParse)[i] == delim))) ||
         ((insidequotes == 1) &&
          ((*toParse)[i] != '\0'))) {
    if (((*toParse)[i] == '\'') || ((*toParse)[i] == '"')) {
      insidequotes = !insidequotes;
    }
    i++;
  }

  //set the token string to point to first character in the token
  (*toParse) = &(*toParse)[i];
  //set the offset
  i = 0;

  //look for the end of token (e.g. another delim or end of string
  insidequotes = 0;
  while (((insidequotes == 0) &&
         (((*toParse)[i] != '\0') &&
          ((*toParse)[i] != delim))) ||
         ((insidequotes == 1) &&
          ((*toParse)[i] != '\0'))) {
    if (((*toParse)[i] == '\'') || ((*toParse)[i] == '"')) {
      insidequotes = !insidequotes;
    }
    i++;
  }

  result = malloc((i)*sizeof(char));
  strncpy(result, (*toParse), i);
  result[i] = '\0';

  (*toParse) = &(*toParse)[i]; 

  return result;
}

/* removeNewLines: Starts at the end of a string and replaces any newlines with
 *                 null charactars, effectively removing trailing newlines.
 * char **str: a pointer to the string
 * int charcount: the number of characters in the string
 */
void removeNewLines(char **str, int charcount) {
  int i = charcount;

  //loop while the current charactar is eitehr a null or newline
  while ((((*str)[i] == '\n') || ((*str)[i] == '\0')) && (i >= 0)) {
    (*str)[i] = '\0';   //replace the newline or null
    i--;		//decrement down
  }
}

char* removeQuotations(char **str) {
  int charcount = 0;
  char *newstring;
  char quotechar;
  int index=0; 

  while ((*str)[charcount] != '\0') charcount++;

  //printf("charcount: %d\n", charcount);
  (*str) = &(*str)[1];
  newstring = malloc((charcount-1)*sizeof(char));
  strncpy(newstring, (*str), charcount-2);
  newstring[charcount-1] = '\0';
  (*str) = newstring;
  return newstring; 
}

/* endsWithSpaceOrTab: returns 1 for true and 0 for false */
int endsWithSpaceOrTab(char *string) {
  int i = 0;

  while (string[i] != '\0') { i++; }

  if ((string[i-1] == ' ') || (string[i-1] == '\t')) {
    return 1;
  } else {
    return 0;
  }
}


char* makeStringFromArguments(char **args, int argcount, char delim) {
  int i;
  char* result;
  int resultlength = 0;
  char delimstring[1];

  delimstring[0] = delim;

  for (i=0; i<argcount; i++)
    resultlength += strlen(args[0]) + 1;
 
  result = malloc(resultlength*sizeof(char));
  strcpy(result, args[0]);

  for (i=1; i<argcount; i++) {
    strncat(result, delimstring, 1);
    strncat(result, args[i], 256);
  }

  return result;
}

#endif
