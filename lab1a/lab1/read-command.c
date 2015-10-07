// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/************ types of tokens ************/
typedef enum token_type
{
	TOKEN_TYPE_AND,
	TOKEN_TYPE_OR,
	TOKEN_TYPE_PIPE,
	TOKEN_TYPE_
} token_type;

/************ node ************/
typedef struct node_t 
{
	struct node_t * next;
	struct node_t * previous;
	union 
	{
		char *word;
		command_t command;
	};
} node_t;

/************ stack ************/
typedef struct stack_t {
	size_t size;
	node_t *head;
	node_t *end;
} stack_t;

struct command_stream
{
	stack_t *commands_list; // contains command_t
};

/***************** helper functions *****************/
void command_indented_print (int indent, command_t c);

int is_valid (int c);
int is_special (int c);
int is_operator (char *word);
int is_io_redir (char *word);
int get_precedence (char *word);

char *get_stream_from_input (int (*get_next_byte) (void *), 
	void *get_next_byte_argument);
void test_stream (int (*get_next_byte) (void *), 
	void *get_next_byte_argument);


/************** stack functions **************/
stack_t *create_stack ();
void destroy_stack (stack_t *stack);
size_t size_of_stack (stack_t *stack);
int is_empty (stack_t *stack);

char *get_top_word (stack_t *stack);
void push_end_word (stack_t *stack, char *val);
void push_front_word (stack_t *stack, char *val);
char *pop_front_word (stack_t *stack);
char *pop_end_word (stack_t *stack);

void push_end_command (stack_t *stack, command_t command);
void push_front_command (stack_t *stack, command_t command);
command_t pop_front_command (stack_t *stack);
command_t pop_end_command (stack_t *stack);

void print_stack_word (stack_t *stack);
void print_stack_command (stack_t *stack);
void test_stack_word ();
void test_stack_command ();
command_t get_command (enum command_type type);
/*********************************************/





command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
	// test_stack_command ();
	// test_stack_word ();

	char *stream = get_stream_from_input (get_next_byte, get_next_byte_argument);
	// fprintf(stderr, "%s\n", stream);
	// fprintf(stderr, "----------------------------------\n");

	command_stream_t cs = malloc (sizeof (struct command_stream));
	cs->commands_list = create_stack ();


  /********************* ALGORITHM *********************/

  stack_t *word_stack = create_stack ();

  char *buffer;
  size_t size;
  FILE *buffer_stream = open_memstream (&buffer, &size);

  size_t len = strlen (stream);
  size_t i = 0;
  for (; i < len; i++)
  {
    char cc = stream[i];
    if (cc != ' ')
    {
      fprintf(buffer_stream, "%c", cc);
      fflush (buffer_stream);
    }
    else
    {
      /* processing */
      char *word = malloc (strlen(buffer)+1);
      strcpy (word, buffer);
      push_end_word (word_stack, word);

      free (buffer);
      buffer_stream = open_memstream (&buffer, &size);
    }
  }



  stack_t *command_stack = create_stack ();
  stack_t *operator_stack = create_stack ();

  int input_flag = 0;
  int output_flag = 0;
  int new_command_flag = 0;

  char *stack_word = NULL;
  while (!is_empty (word_stack))
  {
    stack_word = pop_front_word (word_stack);
    // fprintf(stderr, "------------------STACK WORD---------------: %s\n", stack_word);
    // fprintf(stderr, "\n");
    // print_stack_command (command_stack);
    // fprintf(stderr, "\n");
    // print_stack_word (operator_stack);
    // fprintf(stderr, "\n");
    if (input_flag || output_flag)
    {
      command_t c = pop_front_command (command_stack);
      if (input_flag)
      {
        c->input = malloc(strlen(stack_word)+1);
        strcpy(c->input, stack_word);
        input_flag = 0;
      }
      else
      {
        c->output = malloc(strlen(stack_word)+1);
        strcpy(c->output, stack_word);
        output_flag = 0;
      }
      push_front_command (command_stack, c);
    }

    else if (!is_operator(stack_word) && strcmp (stack_word, ";")
      && !is_io_redir (stack_word))
    {
      if (!new_command_flag && !is_empty (command_stack))
      {
        command_t top = pop_front_command (command_stack);
        if (top->type == SIMPLE_COMMAND)
        {
          int count = 0;
          char **word = top->u.word;
          while (*word)
          {
            word++;
            count++;
          }

          char **command_s = malloc( (count+2)*sizeof(char *) );
          int pos = 0;
          for (; pos < count; pos++)
          {
            *(command_s+pos) = malloc(strlen( top->u.word[pos] ) +1 );
            strcpy (*(command_s+pos), top->u.word[pos]);
          }
          *(command_s+pos) = malloc(strlen(stack_word)+1);
          strcpy (*(command_s+pos), stack_word);
          *(command_s+pos+1) = NULL;

          top->u.word = command_s;
          push_front_command (command_stack, top);
        }
        else
        {
          push_front_command (command_stack, top);
        }
      }
      else
      {
        command_t c = malloc (sizeof(struct command));
        c->type = SIMPLE_COMMAND;
        c->input = NULL;
        c->output = NULL;
        char **command_s = malloc(2 * sizeof(char *));
        *command_s = malloc(strlen(stack_word)+1);
        strcpy (*command_s, stack_word);
        *(command_s+1) = NULL;

        c->u.word = command_s;

        push_front_command (command_stack, c);

        new_command_flag = 0;
      }
    }

    else if (is_operator (stack_word))
    {
      if (is_empty (operator_stack))
      {
        push_front_word (operator_stack, stack_word);
      }
      else
      {
        if (get_precedence(stack_word) > get_precedence(get_top_word(operator_stack)))
        {
          push_front_word (operator_stack, stack_word);
        }
        else
        {
          while (get_top_word(operator_stack))
          {
            char *operator = pop_front_word (operator_stack);
            command_t command2 = pop_front_command (command_stack);
            command_t command1 = pop_front_command (command_stack);

            if (!strcmp (operator, "&&"))
            {
              command_t c = malloc (sizeof(struct command));
              c->type = AND_COMMAND;
              c->input = NULL;
              c->output = NULL;
              // c->u.command = malloc (2*sizeof(command_t));
              c->u.command[0] = command1;
              c->u.command[1] = command2;
              push_front_command (command_stack, c);
              // fprintf(stderr, "Created AND COMMAND\n");
            }
            else if (!strcmp (operator, "||"))
            {
              command_t c = malloc (sizeof(struct command));
              c->type = OR_COMMAND;
              c->input = NULL;
              c->output = NULL;
              // c->u.command = malloc (2*sizeof(command_t));
              c->u.command[0] = command1;
              c->u.command[1] = command2;
              push_front_command (command_stack, c);
              // fprintf(stderr, "Created OR COMMAND\n");
            }
            else if (!strcmp (operator, "|"))
            {
              command_t c = malloc (sizeof(struct command));
              c->type = PIPE_COMMAND;
              c->input = NULL;
              c->output = NULL;
              // c->u.command = malloc (2*sizeof(command_t));
              c->u.command[0] = command1;
              c->u.command[1] = command2;
              push_front_command (command_stack, c);
              // fprintf(stderr, "Created PIPE COMMAND\n");
            }

          }
          push_front_word (operator_stack, stack_word);
        }
      }

      new_command_flag = 1;
    }

    else if (is_io_redir (stack_word))
    {
      if (!strcmp (stack_word, "<"))
      {
        input_flag = 1;
      }
      else
      {
        output_flag = 1;
      }
    }

    else if (!strcmp (stack_word, ";"))
    {
      while (!is_empty (operator_stack) && size_of_stack (command_stack) >= 2)
      {
        char *operator = pop_front_word (operator_stack);
        command_t command2 = pop_front_command (command_stack);
        command_t command1 = pop_front_command (command_stack);
        
        if (!strcmp (operator, "&&"))
        {
          command_t c = malloc (sizeof(struct command));
          c->type = AND_COMMAND;
          c->input = NULL;
          c->output = NULL;
          // c->u.command = malloc (2*sizeof(command_t));
          c->u.command[0] = command1;
          c->u.command[1] = command2;
          push_front_command (command_stack, c);
          // fprintf(stderr, "Created AND COMMAND\n");
        }
        else if (!strcmp (operator, "||"))
        {
          command_t c = malloc (sizeof(struct command));
          c->type = OR_COMMAND;
          c->input = NULL;
          c->output = NULL;
          // c->u.command = malloc (2*sizeof(command_t));
          c->u.command[0] = command1;
          c->u.command[1] = command2;
          push_front_command (command_stack, c);
          // fprintf(stderr, "Created OR COMMAND\n");
        }
        else if (!strcmp (operator, "|"))
        {
          command_t c = malloc (sizeof(struct command));
          c->type = PIPE_COMMAND;
          c->input = NULL;
          c->output = NULL;
          // c->u.command = malloc (2*sizeof(command_t));
          c->u.command[0] = command1;
          c->u.command[1] = command2;
          push_front_command (command_stack, c);
          // fprintf(stderr, "Created PIPE COMMAND\n");
        }
      }

      command_t c = pop_front_command (command_stack);
      while (c)
      {
        push_end_command (cs->commands_list, c);
        c = pop_front_command (command_stack);
      }

      // fprintf(stderr, "===============================================\n");
      // print_stack_command (cs->commands_list);
      // fprintf(stderr, "===============================================\n");

      new_command_flag = 1;
    }
  }

  // print_stack_word (word_stack);
  // print_stack_command (cs->commands_list);

  /********************* ALGORITHM *********************/


	free (stream);
  return cs;
}

int returned = 0;

command_t
read_command_stream (command_stream_t s)
{
	command_t c = NULL;
	if (!is_empty (s->commands_list))
		c = pop_front_command (s->commands_list);
  return c;
  
  // if (returned)
  // {
  //   return NULL;
  // }
  // else
  // {
  //   command_t c = malloc (sizeof(struct command));
  //   c->type = SIMPLE_COMMAND;
  //   c->input = "file.c";
  //   c->output = "results.txt";
  //   char **command_s = malloc(3 * sizeof(char *));
  //   *command_s = malloc(strlen("cat")+1);
  //   strcpy (*command_s, "cat");
  //   *(command_s+1) = malloc(strlen("true")+1);
  //   strcpy (*(command_s+1), "true");
  //   *(command_s+2) = NULL;

  //   returned = 1;

  //   c->u.word = command_s;
  //   return c;
  // } 
}

int get_precedence (char *word)
{
  if (word == NULL)
  {
    fprintf(stderr, "NULL word in get_precedence\n");
    return -1;
  }
  if (!strcmp (word, ";"))
  {
    return 1;
  }
  else if (!strcmp (word, "&&") || !strcmp (word, "||"))
  {
    return 2;
  }
  else if (!strcmp (word, "|"))
  {
    return 3;
  }
  return -1;
}

int is_io_redir (char *word)
{
  if (!strcmp(word, "<") || !strcmp(word, ">"))
    return 1;
  return 0;
}

int is_operator (char *word)
{
  if (!strcmp(word, "&&") || !strcmp(word, "||") || !strcmp(word, "|"))
    return 1;
  return 0;
}





/************ stack implementations ************/
/***********************************************/

stack_t *
create_stack ()
{
	stack_t *stack = malloc (sizeof(stack_t));
	stack->size = 0;
	stack->head = NULL;
	stack->end = NULL;
	return stack;
}

void
destroy_stack (stack_t *stack)
{
	while (!is_empty (stack))
	{
		char *val = pop_front_word (stack);
		// free (val);
		print_stack_word (stack);
	}
	free (stack);
}

size_t
size_of_stack (stack_t *stack)
{
	return stack->size;
}

int
is_empty (stack_t *stack)
{
	return stack->size == 0;
}

char *
get_top_word (stack_t *stack)
{
  node_t *head = stack->head;
  if (head)
  {
    char *stack_word = head->word;
    if (stack_word)
    {
      char *word = malloc(strlen(stack_word)+1);
      strcpy (word, stack_word);
      return word;
    }
  }
  
  return NULL;
}

/************ push end word ************/
void 
push_end_word (stack_t *stack, char *val)
{
	if (stack == NULL)
		return;
	
	// when stack is empty
	if (stack->head == NULL)
	{
		stack->head = malloc (sizeof(node_t));
		stack->head->word = malloc(strlen(val)+1);
    strcpy (stack->head->word, val);
		stack->head->next = NULL;
		stack->head->previous = NULL;
		stack->end = stack->head;
		stack->size++;
		return;
	}

	// when stack is not empty
	node_t *new_node = malloc (sizeof(node_t));
  new_node->word = malloc(strlen(val)+1);
  strcpy (new_node->word, val);
	new_node->next = NULL;
	new_node->previous = stack->end;
	new_node->previous->next = new_node;
	stack->end = new_node;
	stack->size++;
	return;
}

/************ push front word ************/
void 
push_front_word (stack_t *stack, char *val)
{
	if (stack == NULL)
		return;

	// when stack is empty
	if (stack->head == NULL)
	{
		stack->head = malloc (sizeof(node_t));
		stack->head->word = malloc(strlen(val)+1);
    strcpy (stack->head->word, val);
		stack->head->next = NULL;
		stack->head->previous = NULL;
		stack->end = stack->head;
		stack->size++;
		return;
	}

	// when stack is not empty
	node_t *new_node = malloc (sizeof(node_t));
	new_node->word = malloc(strlen(val)+1);
  strcpy (new_node->word, val);
	new_node->next = stack->head;
	new_node->previous = NULL;
	new_node->next->previous = new_node;
	stack->head = new_node;
	stack->size++;
	return;
}

/************ pop front word ************/
char *
pop_front_word (stack_t *stack)
{
	if (stack == NULL)
		return NULL;
	// when stack is empty
	if (stack->head == NULL)
		return NULL;

	// when stack is not empty
	char *word = stack->head->word;
	node_t *current = stack->head;

	// if there's only one node left
	if (current->next == NULL)
	{
		stack->head = NULL;
		stack->end = NULL;
	}
	// if there's more than one node left
	else
	{
		current->next->previous = NULL;
		stack->head = current->next;
		current->next = NULL;
	}
	free (current);
	stack->size--;
	return word;
}

/************ pop end word ************/
char *
pop_end_word (stack_t *stack)
{
	if (stack == NULL)
		return NULL;
	// when stack is empty
	if (stack->end == NULL)
		return NULL;

	// when stack is not empty
	char *word = stack->end->word;
	node_t *current = stack->end;

	// if there's only one node left
	if (current->previous == NULL)
	{
		stack->head = NULL;
		stack->end = NULL;
	}
	// if there's more than one node left
	else
	{
		current->previous->next = NULL;
		stack->end = current->previous;
		current->previous = NULL;
	}
	free (current);
	stack->size--;
	return word;
}

/************ push end command ************/
void 
push_end_command (stack_t *stack, command_t val)
{
	if (stack == NULL)
		return;
	
	// when stack is empty
	if (stack->head == NULL)
	{
		stack->head = malloc (sizeof(node_t));
		stack->head->command = val;
		stack->head->next = NULL;
		stack->head->previous = NULL;
		stack->end = stack->head;
		stack->size++;
		return;
	}

	// when stack is not empty
	node_t *new_node = malloc (sizeof(node_t));
	new_node->command = val;
	new_node->next = NULL;
	new_node->previous = stack->end;
	new_node->previous->next = new_node;
	stack->end = new_node;
	stack->size++;
	return;
}

/************ push front command ************/
void 
push_front_command (stack_t *stack, command_t val)
{
	if (stack == NULL)
		return;

	// when stack is empty
	if (stack->head == NULL)
	{
		stack->head = malloc (sizeof(node_t));
		stack->head->command = val;
		stack->head->next = NULL;
		stack->head->previous = NULL;
		stack->end = stack->head;
		stack->size++;
		return;
	}

	// when stack is not empty
	node_t *new_node = malloc (sizeof(node_t));
	new_node->command = val;
	new_node->next = stack->head;
	new_node->previous = NULL;
	new_node->next->previous = new_node;
	stack->head = new_node;
	stack->size++;
	return;
}

/************ pop front command ************/
command_t
pop_front_command (stack_t *stack)
{
	if (stack == NULL)
		return NULL;
	// when stack is empty
	if (stack->head == NULL)
		return NULL;

	// when stack is not empty
	command_t command = stack->head->command;
	node_t *current = stack->head;

	// if there's only one node left
	if (current->next == NULL)
	{
		stack->head = NULL;
		stack->end = NULL;
	}
	// if there's more than one node left
	else
	{
		current->next->previous = NULL;
		stack->head = current->next;
		current->next = NULL;
	}
	free (current);
	stack->size--;
	return command;
}

/************ pop end command ************/
command_t
pop_end_command (stack_t *stack)
{
	if (stack == NULL)
		return NULL;
	// when stack is empty
	if (stack->end == NULL)
		return NULL;

	// when stack is not empty
	command_t command = stack->end->command;
	node_t *current = stack->end;

	// if there's only one node left
	if (current->previous == NULL)
	{
		stack->head = NULL;
		stack->end = NULL;
	}
	// if there's more than one node left
	else
	{
		current->previous->next = NULL;
		stack->end = current->previous;
		current->previous = NULL;
	}
	free (current);
	stack->size--;
	return command;
}

/************ testing ************/

void 
print_stack_word (stack_t *stack)
{
	node_t *current = stack->head;
	fprintf(stderr, "======STACK======\n");
	fprintf(stderr, "SIZE: %d\n", (int)stack->size);
	while (current != NULL)
	{
		fprintf(stderr, "%s\n", current->word);
		current = current->next;
	}
	fprintf(stderr, "=======END=======\n");
}

void 
print_stack_command (stack_t *stack)
{
	node_t *current = stack->head;
	fprintf(stderr, "======STACK======\n");
	fprintf(stderr, "STACK-SIZE: %d\n", (int)stack->size);
	while (current)
	{
    command_t c = current->command;
    fprintf(stderr, "-------COMMAND-------\n");
		// fprintf(stderr, "TYPE: %d\n", (int)current->command->type);
  //   if (c->input)
  //     fprintf(stderr, "INPUT %s\n", c->input);
  //   if (c->output)
  //     fprintf(stderr, "OUTPUT %s\n", c->output);
  //   switch (c->type)
  //   {
  //     case SIMPLE_COMMAND:
  //     {
  //       char **words = c->u.word;
  //       int cc = 0;
  //       while (*words)
  //       {
  //         fprintf(stderr, "%d : %s\n", cc, *words);
  //         cc++;
  //         words++;
  //       }
  //       break;
  //     }

  //     case AND_COMMAND:
  //     case SEQUENCE_COMMAND:
  //     case OR_COMMAND:
  //     case PIPE_COMMAND:
  //     {
  //       command_t c1 = c->u.command[0];
  //       char **words = c1->u.word;
  //       int cc = 0;
  //       while (*words)
  //       {
  //         fprintf(stderr, "%d : %s\n", cc, *words);
  //         cc++;
  //         words++;
  //       }

  //       command_t c2 = c->u.command[1];
  //       words = c2->u.word;
  //       cc = 0;
  //       while (*words)
  //       {
  //         fprintf(stderr, "%d : %s\n", cc, *words);
  //         cc++;
  //         words++;
  //       }
  //       break;
  //     }

  //     default: break;

  //   }

    command_indented_print (4, c);
    
    fprintf(stderr, "-------COMMAND END-------\n\n");
		current = current->next;
	}
	fprintf(stderr, "=======END=======\n");
}

void 
test_stack_word () 
{
	stack_t *stack = create_stack ();

	push_end_word (stack, "cc");
	print_stack_word (stack);
	push_end_word (stack, "aa");
	print_stack_word (stack);
	push_front_word (stack, "bb");
	print_stack_word (stack);
	push_end_word (stack, "gg");
	print_stack_word (stack);
	push_front_word (stack, "fuck This!!!");
	print_stack_word (stack);
	
	char *word = NULL;
	fprintf(stderr, "popped: %s\n", (word = pop_front_word(stack)));
	print_stack_word (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_end_word(stack)));
	print_stack_word (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_end_word(stack)));
	print_stack_word (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_front_word(stack)));
	print_stack_word (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_front_word(stack)));
	print_stack_word (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_end_word(stack)));
	print_stack_word (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_front_word(stack)));
	print_stack_word (stack);

	destroy_stack (stack);
	free (word);
}

void
test_stack_command ()
{
	stack_t *stack = create_stack ();

	push_front_command (stack, get_command (AND_COMMAND));
	print_stack_command (stack);
	push_front_command (stack, get_command (OR_COMMAND));
	print_stack_command (stack);
	push_front_command (stack, get_command (SUBSHELL_COMMAND));
	print_stack_command (stack);
	push_front_command (stack, get_command (SIMPLE_COMMAND));
	print_stack_command (stack);

	command_t c = NULL;
	c = pop_front_command (stack);
	print_stack_command (stack);
	free (c);
	c = pop_front_command (stack);
	print_stack_command (stack);
	free (c);
	c = pop_front_command (stack);
	print_stack_command (stack);
	free (c);
	c = pop_front_command (stack);
	print_stack_command (stack);
	free (c);
	c = pop_front_command (stack);
	print_stack_command (stack);
	free (c);
	c = pop_front_command (stack);
	print_stack_command (stack);
	free (c);
	c = pop_front_command (stack);
	print_stack_command (stack);
	free (c);
	c = pop_front_command (stack);
	print_stack_command (stack);
	free (c);

	destroy_stack (stack);
}

command_t get_command (enum command_type type)
{
	command_t c = malloc(sizeof(struct command));
	c->type = type;
	return c;
}

/*************************************/
/*************************************/



/************** helpers **************/
/*************************************/

char *
get_stream_from_input (int (*get_next_byte) (void *), 
	void *get_next_byte_argument)
{
	FILE *stream;
	size_t size;
	char *buffer;
	stream = open_memstream (&buffer, &size);

	/********** read input into a buffer **********/
	int c;
	while ((c = (*get_next_byte) (get_next_byte_argument)) != EOF)
	{
		fprintf(stream, "%c", c);
		fflush (stream);
	}
	fclose (stream);


	/************ reformat (insert spaces) ************/
	// state flags
	int comment_flag = 0;

	unsigned int length = strlen (buffer);
	char *script = (char *) malloc (3*length);
	unsigned int pos = 0;
	unsigned int i = 0;
	for (; i < length; i++)
	{
		// realloc if script is full
		if (pos >= length-10)
			script = (char *) realloc (script, 4*length);

		int cc = buffer[i];

		if (comment_flag)
		{
			if (cc != '\n')
			{
				script[pos] = cc;
				pos++;
				continue;
			}
			else
			{
				script[pos] = cc;
				pos++;
				comment_flag = 0;
				continue;
			}
		}

		if (is_valid (cc) || cc == ' ')
		{
			if (cc == '#')
			{
				comment_flag = 1;
			}

			else if (pos > 0 && (script[pos-1] == '|' || script[pos-1] == '<' 
				|| script[pos-1] == '>' || script[pos-1] == '(' || script[pos-1] == ')'))
			{
				script[pos] = ' ';
				pos++;
			}

			script[pos] = cc;
			pos++;

			if ((i+2) < (length-1))
			{
				if (buffer[i+1] == '&' && buffer[i+2] == '&')
				{
					script[pos] = ' ';
					pos++;
					script[pos] = '&';
					pos++;
					script[pos] = '&';
					pos++;
					script[pos] = ' ';
					pos++;
					i += 2;
				}
			}
		}

		else if (cc == '\n')
		{
			if (script[pos-1] != '\n')
			{
				if (script[pos-1] == ' ')
				{
					script[pos-1] = cc;
				}
				else
				{
					script[pos] = cc;
					pos++;
				}
			}
		}

		else if (is_special (cc))
		{
			switch (cc)
			{
				case '(':
				case ')':
				case '<':
				case '>':
				{
					if (script[pos-1] != ' ' && script[pos-1] != '\n')
					{
						script[pos] = ' ';
						pos++;
						script[pos] = cc;
						pos++;
					}
					else
					{
						script[pos] = cc;
						pos++;
					}
					break;
				}

				case '|':
				{
					if (script[pos-1] != ' ' && script[pos-1] != '\n' && script[pos-1] != '|')
					{
						script[pos] = ' ';
						pos++;
						script[pos] = cc;
						pos++;
					}
					else
					{
						script[pos] = cc;
						pos++;
					}
					break;
				}

				case '&':
				{
					script[pos] = cc;
					pos++;
					break;
				}
			}
		}
	}

	script[pos] = '\0';


	/************ trim extra spaces ************/
	free (buffer);
	length = strlen (script);
	buffer = (char *) malloc (2*length);
	pos = 0;
	comment_flag = 0;
	for (i = 0; i < length; i++)
	{
		int cc = script[i];

		if (comment_flag)
		{
			if (cc != '\n')
			{
				buffer[pos] = cc;
				pos++;
				continue;
			}
			else
			{
				buffer[pos] = cc;
				pos++;
				comment_flag = 0;
				continue;
			}
		}

		if (cc == '#')
		{
			comment_flag = 1;
			buffer[pos] = cc;
			pos++;
		}

		else if (cc == ' ')
		{
			if (buffer[pos-1] == ' ' || buffer[pos-1] == '\n')
			{
				continue;
			}
			else
			{
				buffer[pos] = cc;
				pos++;
			}
		}

		else
		{
			buffer[pos] = cc;
			pos++;
		}
	}

	buffer[pos] = '\0';


	/************ trim extra newlines ************/
	free (script);
	length = strlen (buffer);
	script = (char *) malloc (2*length);
	pos = 0;
	comment_flag = 0;
	int special_flag = 0;
	for (i = 0; i < length; i++)
	{
		int cc = buffer[i];

		if (comment_flag)
		{
			if (cc != '\n')
			{
				script[pos] = cc;
				pos++;
				continue;
			}
			else
			{
				script[pos] = cc;
				pos++;
				comment_flag = 0;
				continue;
			}
			special_flag = 0;
		}

		if (cc == '#')
		{
			comment_flag = 1;
			script[pos] = cc;
			pos++;
			special_flag = 0;
		}

		else if (cc == '&' && script[pos-1] == '&')
		{
			special_flag = 1;
			script[pos] = cc;
			pos++;
		}

		else if (cc == '|')
		{
			special_flag = 1;
			script[pos] = cc;
			pos++;
		}

		else if (cc == '\n')
		{
			if (special_flag && is_valid (buffer[pos+1]))
			{
				script[pos] = ' ';
				pos++;
			}

			else if (buffer[pos+1] == '|')
			{
				script[pos] = ' ';
				pos++;
				special_flag = 1;
			}

			else if (buffer[pos+1] == '&' && buffer[pos+2] == '&')
			{
				script[pos] = ' ';
				pos++;
				special_flag = 1;
			}

			else
			{
				script[pos] = cc;
				pos++;
			}
			special_flag = 0;
		}

		else
		{
			script[pos] = cc;
			pos++;
			special_flag = 0;
		}
	}

	script[pos] = '\0';
	

	/************ transform newlines into ';'' ************/
	free (buffer);
	length = strlen (script);
	buffer = (char *) malloc (2*length);
	pos = 0;
	comment_flag = 0;
	for (i = 0; i < length; i++)
	{
		int cc = script[i];

		if (comment_flag)
		{
			if (cc != '\n')
			{
				continue;
			}
			else
			{
				comment_flag = 0;
				continue;
			}
		}

		if (cc == '#')
		{
			comment_flag = 1;
		}

		else if (cc == '\n')
		{
			buffer[pos] = ' ';
			pos++;
			buffer[pos] = ';';
			pos++;
			buffer[pos] = ' ';
			pos++;
		}

		else
		{
			buffer[pos] = cc;
			pos++;
		}
	}

	buffer[pos] = '\0';
	free (script);

	return buffer ;
}

int
is_valid (int c)
{
	if (isalpha (c) || isdigit (c))
		return 1;

	switch (c)
	{
		case '!':
		case '%':
		case '+':
		case '-':
		case ',':
		case '.':
		case '/':
		case ':':
		case '@':
		case '^':
		case '_':
		case '#':
			return 1;
			break;
		default: break;
	}

	return 0;
}

int
is_special (int c)
{
	switch (c)
	{
		case ';':
		case '|':
		case '&':
		case '(':
		case ')':
		case '<':
		case '>':
			return 1;
			break;
		default: break;
	}

	return 0;
}

void
test_stream (int (*get_next_byte) (void *), 
	void *get_next_byte_argument)
{
	char *stream = get_stream_from_input (get_next_byte, get_next_byte_argument);
	fprintf(stderr, "%s\n", stream);
	free (stream);
}


void
command_indented_print (int indent, command_t c)
{
  switch (c->type)
    {
    case AND_COMMAND:
    case SEQUENCE_COMMAND:
    case OR_COMMAND:
    case PIPE_COMMAND:
      {
  command_indented_print (indent + 2 * (c->u.command[0]->type != c->type),
        c->u.command[0]);
  static char const command_label[][3] = { "&&", ";", "||", "|" };
  fprintf (stderr, " \\\n%*s%s\n", indent, "", command_label[c->type]);
  command_indented_print (indent + 2 * (c->u.command[1]->type != c->type),
        c->u.command[1]);
  break;
      }

    case SIMPLE_COMMAND:
      {
  char **w = c->u.word;
  fprintf (stderr, "%*s%s", indent, "", *w);
  while (*++w)
    fprintf (stderr, " %s", *w);
  break;
      }

    case SUBSHELL_COMMAND:
      fprintf (stderr, "%*s(\n", indent, "");
      command_indented_print (indent + 1, c->u.subshell_command);
      fprintf (stderr, "\n%*s)", indent, "");
      break;

    default:
      abort ();
    }

  if (c->input)
    fprintf (stderr, "<%s", c->input);
  if (c->output)
    fprintf (stderr, ">%s", c->output);
}