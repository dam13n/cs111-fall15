// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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
void print_error (char *error_message, int line_number);
void command_indented_print (int indent, command_t c);

int is_valid (int c);
int is_special (int c);
int is_operator (char *word);
int is_io_redir (char *word);
int get_precedence (char *word);

char *get_stream_from_input (int (*get_next_byte) (void *), 
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
	char *stream = get_stream_from_input (get_next_byte, get_next_byte_argument);

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
              c->u.command[0] = command1;
              c->u.command[1] = command2;
              push_front_command (command_stack, c);
            }
            else if (!strcmp (operator, "||"))
            {
              command_t c = malloc (sizeof(struct command));
              c->type = OR_COMMAND;
              c->input = NULL;
              c->output = NULL;
              c->u.command[0] = command1;
              c->u.command[1] = command2;
              push_front_command (command_stack, c);
            }
            else if (!strcmp (operator, "|"))
            {
              command_t c = malloc (sizeof(struct command));
              c->type = PIPE_COMMAND;
              c->input = NULL;
              c->output = NULL;
              c->u.command[0] = command1;
              c->u.command[1] = command2;
              push_front_command (command_stack, c);
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
          c->u.command[0] = command1;
          c->u.command[1] = command2;
          push_front_command (command_stack, c);
        }
        else if (!strcmp (operator, "||"))
        {
          command_t c = malloc (sizeof(struct command));
          c->type = OR_COMMAND;
          c->input = NULL;
          c->output = NULL;
          c->u.command[0] = command1;
          c->u.command[1] = command2;
          push_front_command (command_stack, c);
        }
        else if (!strcmp (operator, "|"))
        {
          command_t c = malloc (sizeof(struct command));
          c->type = PIPE_COMMAND;
          c->input = NULL;
          c->output = NULL;
          c->u.command[0] = command1;
          c->u.command[1] = command2;
          push_front_command (command_stack, c);
        }
      }

      command_t c = pop_front_command (command_stack);
      while (c)
      {
        push_end_command (cs->commands_list, c);
        c = pop_front_command (command_stack);
      }

      new_command_flag = 1;
    }
  }

  /********************* ALGORITHM *********************/


	free (stream);
  return cs;
}

command_t
read_command_stream (command_stream_t s)
{
	command_t c = NULL;
	if (!is_empty (s->commands_list))
		c = pop_front_command (s->commands_list);
  return c;
}

int get_precedence (char *word)
{
  if (word == NULL)
  {
    fprintf(stderr, "NULL word in get_precedence\n");
    return -1;
  }

  if (!strcmp (word, ";"))
    return 1;
  else if (!strcmp (word, "&&") || !strcmp (word, "||"))
    return 2;
  else if (!strcmp (word, "|"))
    return 3;
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
    command_indented_print (4, c);
    fprintf(stderr, "-------COMMAND END-------\n\n");
		current = current->next;
	}
	fprintf(stderr, "=======END=======\n");
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


  /************ check for errors in formating ************/
  int line_count = 1;
  int open_paren_count = 0;
  int comment_flag = 0;

  unsigned int pos = 0;
  unsigned int i = 0;
  unsigned int length = strlen (buffer);

  for (; i < length; i++)
  {
    int cc = buffer[i];

    if (comment_flag)
    {
      if (cc == '\n')
      {
        comment_flag = 0;
        continue;
      }
      else
      {
      continue;
      }
    }

    if (cc == '#')
    {
      comment_flag = 1;
      continue;
    }

    if (!is_valid (cc) && !is_special (cc)
      && cc != ' ' && cc != '\n')
    {
      print_error ("Invalid character.", line_count);
    }

    if (cc == '|')
    {
      if (i == 0)
      {
        print_error ("'|' at the start.", line_count);
      }
      else if (i == (length-2))
      {
        print_error ("'|' at the end.", line_count);
      }
      else if ((i+2) < (length-1))
      {
        if (buffer[i+1] == '|' && buffer[i+2] == '|')
        {
          print_error ("Excess '|'.", line_count);
        }
      }
    }

    else if (cc == '&')
    {
      if (i == 0)
      {
        print_error ("'&' at the start.", line_count);
      }
      else if (i == (length-2))
      {
        print_error ("'&' at the end.", line_count);
      }
      else if ((i+2) < (length-1))
      {
        if (buffer[i+1] == '&' && buffer[i+2] == '&')
        {
          print_error ("Excess '&'.", line_count);
        }
      }
    }

    else if (cc == ';')
    {
      if (i == 0)
      {
        print_error ("';' at the start.", line_count);
      }
      else if ((i+1) < (length-1))
      {
        if (buffer[i+1] == ';')
        {
          print_error ("Excess ';'.", line_count);
        }
      }
    }

    else if (cc == '<')
    {
      if (i == 0)
      {
        print_error ("'<' at the start.", line_count);
      }
      else if (i == (length-2))
      {
        print_error ("'<' at the end.", line_count);
      }
      else if ((i+2) < (length-1))
      {
        if (buffer[i+1] == '<' && buffer[i+2] == '<')
        {
          print_error ("Invalid '<<<'.", line_count);
        }
      }
    }

    else if (cc == '>')
    {
      if (i == 0)
      {
        print_error ("'>' at the start.", line_count);
      }
      else if (i == (length-2))
      {
        print_error ("'>' at the end.", line_count);
      }
      else if ((i+2) < (length-1))
      {
        if (buffer[i+1] == '>' && buffer[i+2] == '>')
        {
          print_error ("Invalid '>>>'.", line_count);
        }
      }
    }

    else if (cc == '\n')
    {
      if (i == (length-1))
        continue;

      line_count++;

      unsigned int temp = i+1;
      for (; temp < length-1; temp++)
      {
        if (buffer[temp] == ' ' || buffer[temp] == '\n')
        {
          continue;
        }
        else
        {
          if (!is_valid (buffer[temp]))
          {
            print_error ("Invalid character after newline", line_count);
          }
          else
          {
            break;
          }
        }
      }
    }

    else if (cc == '(')
      open_paren_count++;
    else if (cc == ')')
      open_paren_count--;
  }

  if (open_paren_count != 0)
    print_error ("Non-matching parentheses.", line_count);


	/************ reformat (insert spaces) ************/
	// state flags
  comment_flag = 0;

	char *script = (char *) malloc (3*length);
  pos = 0;
  i = 0;

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
        line_count++;
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

			if ((i+3) < (length-1))
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
      line_count++;
		}

		else if (is_special (cc))
		{
			switch (cc)
			{
				case '<':
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

  if (c == '!' || c == '%' || c == '+' || c == '-'
    || c == ',' || c == '.' || c == '/' || c == ':'
    || c == '@' || c == '^' || c == '_' || c == '#')
  {
    return 1;
  }
	return 0;
}

int
is_special (int c)
{
  if (c == ';' || c == '|' || c == '&' || c == '(' 
    || c == ')' || c == '<' || c == '>')
  {
    return 1;
  }
	return 0;
}


void
print_error (char *error_message, int line_number)
{
  fprintf(stderr, "Line %d: %s\n", line_number, error_message);
  exit (1);
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
