// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

/* Helper functions with tests */

typedef enum token_type
  {
    TOKEN_WORD,
    TOKEN_NEWLINE,
    TOKEN_AND,
    TOKEN_SEQUENCE,
    TOKEN_OR,
    TOKEN_PIPE,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_REDIR_INPUT,
    TOKEN_REDIR_OUTPUT
  } token_type;

struct token
{
  enum token_type type;
  char *word;
};

typedef struct token *token_t;

token_type get_token_type (char *word);
token_t get_token_t_from_word (char *word);


/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
struct command_stream
{
  //struct command_t *commands;
  node_t nodes[100];
};


///////////////////////////// Node //////////////////////////////////////

int main(){


	//Creates the two stack for processing
	stack_t *operandStack = create_stack();
	stack_t *operatorStack = create_stack();

	//Start reading from the commandStream
	command_t a = read_command_stream (commandStream);

	//Continue reading until the end of the command stream
	while(a!=NULL){
		
		//If encounters a new regular command
		if(a->type != SIMPLE_COMMAND && a->type != SUBSHELL_COMMAND){
			//Push the first operand on the operand stack
			push_front_command(operandStack, a->command[0]);

			//If the operator stack is empty, push operator on to operator stack
			if(is_empty(*operatorStack == NULL)){
				push_front_word(operatorStack, operatorInString(a->type));
			}

			//Else ...
			else{
				//Get the top operator
				char* operatorTop = pop_front_word(operatorStack);
				push_front_word (operatorStack, operatorTop);

				//While the top of the stack is not "(" and precedence new operator <= top operator
				while(operatorTop != "(" &&  (strcmp(operatorInString(a->type), operatorTop)<=0)){

					//Pop new operator and remake new first and second commands
					char* newOperator = pop_front_word(operatorStack);
					command_t second_command = operandStack.pop_front_command;
					command_t first_command = operandStack.pop_front_command;

					//Combine the two command into a new command
					command_t newCommand = malloc(sizeof(command_t));
					newCommand->type = operatorInEnum(newOperator);
					newCommand->u[0] = first_command;
					newCommand->u[2] = second_command;
					free(first_command);
					free(second_command);

					//Add the new command into the operand stack
					push_front_command(operandStack, newCommand);

					//!!!!!!!!!!not sure if we can push null back on
					//Maybe we need a peek or top function?
					operatorTop = pop_front_word(operatorStack);
					push_front_word (operatorStack, operatorTop);
					if(operatorTop = NULL){
						break;
					}
				}
				push_front_word(operatorStack, operatorInString(a->type));
			}
		}

	}
}
	

/******************* Get the operator back in string *********************/
char* operatorInString(command_type commands){
	switch(commands){
    case AND_COMMAND:
    	return "&&";     
    case SEQUENCE_COMMAND:    
    	return ";";
    case OR_COMMAND:
    	return "||";
    case PIPE_COMMAND:
    	return "|";      
    }
  }

command_type operatorInEnum(char* commands){
	switch(commands){
		case "&&":
			return AND_COMMAND;
		case ";":
			return SEQUENCE_COMMAND;
		case "||":
			return OR_COMMAND;
		case "|":
			return PIPE_COMMAND;
	}
}




typedef struct node_t {
    char* val;
    struct node_t * next;
    struct node_t * previous;
} node_t;



//////////////////////// Functions //////////////////////////////////////
void print_list(node_t * head) {
    node_t * current = head;
    
    while (current != NULL) {
        printf("%s\n", current->val);
        current = current->next;
    }
}

//push end
void pushEnd(node_t ** head, char* val) {
    
    if((*head) == NULL){
        (*head) = malloc(sizeof(node_t));
        (*head)->val = malloc(strlen(val+1));
        strcpy((*head)->val, val);
        (*head)->next = NULL;
        (*head)->previous = NULL;
        return;
    }
    
    node_t * current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(node_t));
    current->next->val = malloc(strlen(val+1));
    strcpy(current->next->val, val);
    current->next->next = NULL;
    current->next->previous = current;
    return;
}

//push front
void pushFront(node_t ** head, char* val) {
    
    if((*head) == NULL){
        (*head) = malloc(sizeof(node_t));
        (*head)->val = malloc(strlen(val+1));
        strcpy((*head)->val, val);
        (*head)->next = NULL;
        (*head)->previous = NULL;
        return;
    }
    
    node_t * extraNode;
    extraNode = malloc(sizeof(node_t));
    extraNode->val = malloc(strlen(val+1));
    strcpy(extraNode->val, val);
    extraNode->next = *head;
    extraNode->next->previous = extraNode;
    extraNode->previous = NULL;
    *head = extraNode;
    return;
}


//pop front
void popFront(node_t ** head) {
    if (*head == NULL) {
        return;
    }
    node_t * nextNode = NULL;
    nextNode = (*head)->next;
    if(nextNode!=NULL){
        nextNode->previous = NULL;
    }
    free((*head)->val);
    free(*head);
    *head = nextNode;
}

//pop end

void popEnd(node_t ** head) {
    if (*head == NULL){
        return;
    }
    if ((*head)->next == NULL) {
        free(*head);
        *head = NULL;
        return;
    }
    
    node_t * current = *head;
    
    while (current->next->next != NULL) {
        current = current->next;
    }
    free(current->next);
    current->next = NULL;
    return;
}


/////////////////// Xcode Debugging ////////////////////////////
// int main(int argc, const char * argv[]) {
//     // insert code here...
//     node_t * head = NULL;
//     pushFront(&head, "c");
//     //pushEnd(&head, "ddd");
//     pushFront(&head, "bb");
//     pushFront(&head, "a");
//     pushEnd(&head, "e");
//     popEnd(&head);
//     popFront(&head);
//     popFront(&head);
//     popFront(&head);
//     popFront(&head);
//     popFront(&head);
//     popEnd(&head);
//     print_list(head);

    
    
//     return 0;
// }



//////////////////////////////////////////////////////////////////////////////////////////

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  
  FILE *stream;
  size_t size;
  char *word;
  stream = open_memstream (&word, &size);

  int is_special;
  int special_char;

  int c;
  while ((c = (*get_next_byte) (get_next_byte_argument)) != EOF)
    {      
      if (c != '\n' && c != ' ')
	{
	  if (is_special)
	    {
	      if (special_char == c)
		{
		  token_t token = get_token_t_from_word (word);
		  fprintf (stderr, "type: %d, word: %s\n", 
		       (*token).type, (*token).word);
		  free (token);
		  free (word);
		  stream = open_memstream (&word, &size);
		  is_special = 0;
		}
	      else
		{
		  is_special = 0;
		}
	      fprintf (stream, "%c", special_char);
	      fflush (stream);
	      fprintf (stream, "%c",  c);
	      fflush (stream);
	    }
	  else if (c == '&' || c == '|')
	    {
	      is_special = 1;
	      special_char = c;
	    }
	  else
	    {
	      fprintf (stream, "%c",  c);
	      fflush (stream);
	    }
	}
      else
	{
	  if (is_special)
	    {
	      fprintf (stream, "%c", special_char);
	      fflush (stream);
	      is_special = 0;
	    }

	  if (strlen (word) > 0)
	    {
	      token_t token = get_token_t_from_word (word);
	      fprintf (stderr, "type: %d, word: %s\n", 
		       (*token).type, (*token).word);
	      free (token);
	    }
	  free (word);
	  stream = open_memstream (&word, &size);
	}
    }

  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  
  return 0;
}


/* Helper functions with tests*/

token_t
get_token_t_from_word (char *word)
{
  token_t token = (token_t) malloc(sizeof(token_t));
  (*token).type = get_token_type (word);
  (*token).word = (char *) malloc(sizeof(word));
  strcpy((*token).word, word);
  return token;
}

token_type
get_token_type (char *word)
{
  if (!strcmp (word, "\n"))
    return TOKEN_NEWLINE;
  else if (!strcmp (word, "&&"))
    return TOKEN_AND;
  else if (!strcmp (word, ";"))
    return TOKEN_SEQUENCE;
  else if (!strcmp (word, "||"))
    return TOKEN_OR;
  else if (!strcmp (word, "|"))
    return TOKEN_PIPE;
  else if (!strcmp (word, "("))
    return TOKEN_OPEN_PAREN;
  else if (!strcmp (word, ")"))
    return TOKEN_CLOSE_PAREN;
  else if (!strcmp (word, "<"))
    return TOKEN_REDIR_INPUT;
  else if (!strcmp (word, ">"))
    return TOKEN_REDIR_OUTPUT;
  return TOKEN_WORD;
}
