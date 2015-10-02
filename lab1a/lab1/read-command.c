// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

/************ node ************/
typedef struct node_t {
		char* val;
		struct node_t * next;
		struct node_t * previous;
} node_t;

/************ stack ************/
typedef struct stack_t {
	size_t size;
	node_t *head;
	node_t *end;
} stack_t;

struct command_stream
{
	stack_t *commands_list;
};

int is_valid (int c);
int is_special (int c);

char *get_stream_from_input (int (*get_next_byte) (void *), 
	void *get_next_byte_argument);
void test_stream (int (*get_next_byte) (void *), 
	void *get_next_byte_argument);


/************** stack functions **************/
stack_t *create_stack ();
void destroy_stack (stack_t *stack);
size_t size_of_stack (stack_t *stack);
int is_empty (stack_t *stack);
void push_end (stack_t *stack, char *val);
void push_front (stack_t *stack, char *val);
char *pop_front (stack_t *stack);
char *pop_end (stack_t *stack);

void print_stack (stack_t *stack);
void test_stack ();
/*********************************************/







command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
	char *stream = get_stream_from_input (get_next_byte, get_next_byte_argument);
	fprintf(stderr, "%s\n", stream);
	fprintf(stderr, "----------------------------------\n");

	// command_stream_t cs = malloc (sizeof (command_stream))
	// cs->commands_list = create_stack ();
	// cs->commands_list

	free (stream);
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
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
		char *val = pop_front (stack);
		// free (val);
		print_stack (stack);
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

/************ push end ************/
void 
push_end (stack_t *stack, char *val)
{
	if (stack == NULL)
		return;
	
	// when stack is empty
	if (stack->head == NULL)
	{
		stack->head = malloc (sizeof(node_t));
		stack->head->val = val;
		stack->head->next = NULL;
		stack->head->previous = NULL;
		stack->end = stack->head;
		stack->size++;
		return;
	}

	// when stack is not empty
	node_t *new_node = malloc (sizeof(node_t));
	new_node->val = val;
	new_node->next = NULL;
	new_node->previous = stack->end;
	new_node->previous->next = new_node;
	stack->end = new_node;
	stack->size++;
	return;
}

/************ push front ************/
void 
push_front (stack_t *stack, char *val)
{
	if (stack == NULL)
		return;

	// when stack is empty
	if (stack->head == NULL)
	{
		stack->head = malloc (sizeof(node_t));
		stack->head->val = val;
		stack->head->next = NULL;
		stack->head->previous = NULL;
		stack->end = stack->head;
		stack->size++;
		return;
	}

	// when stack is not empty
	node_t *new_node = malloc (sizeof(node_t));
	new_node->val = val;
	new_node->next = stack->head;
	new_node->previous = NULL;
	new_node->next->previous = new_node;
	stack->head = new_node;
	stack->size++;
	return;
}

/************ pop front ************/
char *
pop_front (stack_t *stack)
{
	if (stack == NULL)
		return NULL;
	// when stack is empty
	if (stack->head == NULL)
		return NULL;

	// when stack is not empty
	char *word = stack->head->val;
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

/************ pop end ************/
char *
pop_end (stack_t *stack)
{
	if (stack == NULL)
		return NULL;
	// when stack is empty
	if (stack->end == NULL)
		return NULL;

	// when stack is not empty
	char *word = stack->end->val;
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

/************ testing ************/

void 
print_stack (stack_t *stack)
{
	node_t *current = stack->head;
	fprintf(stderr, "======STACK======\n");
	fprintf(stderr, "SIZE: %d\n", (int)stack->size);
	while (current != NULL)
	{
		fprintf(stderr, "%s\n", current->val);
		current = current->next;
	}
	fprintf(stderr, "=======END=======\n");
}

void 
test_stack () 
{
	stack_t *stack = create_stack ();

	push_end (stack, "cc");
	print_stack (stack);
	push_end (stack, "aa");
	print_stack (stack);
	push_front (stack, "bb");
	print_stack (stack);
	push_end (stack, "gg");
	print_stack (stack);
	push_front (stack, "fuck This!!!");
	print_stack (stack);
	
	char *word = NULL;
	fprintf(stderr, "popped: %s\n", (word = pop_front(stack)));
	print_stack (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_end(stack)));
	print_stack (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_end(stack)));
	print_stack (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_front(stack)));
	print_stack (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_front(stack)));
	print_stack (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_end(stack)));
	print_stack (stack);
	fprintf(stderr, "popped: %s\n", (word = pop_front(stack)));
	print_stack (stack);

	destroy_stack (stack);
	free (word);
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
