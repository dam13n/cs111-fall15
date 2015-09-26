// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

/* Helper functions with tests */

enum token_type
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
    TOKEN_REIDR_OUTPUT
  };

struct token
{
  enum token_type type;
  char *word;
};

typedef struct token *token_t;

// 

// Returns 1 if input is operator
int isOperator (char *input);
void testIsOperator (void);

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
struct command_stream
{
  struct command_t *commands;
};

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

  // fprintf (stderr, "Called make_command_stream.\n");
  int c;
  while ((c = (*get_next_byte) (get_next_byte_argument)) != EOF)
    {
      // fprintf (stderr, "Called get_next_byte. Got char: %c\n", c);
      
      if (c != '\n' && c != ' ')
	{
	  fprintf (stream, "%c",  c);
	  fflush (stream);
	}
      else
	{
	  if (strlen (word) > 0)
	    {
	      if (isOperator (word))
		{
		  fprintf (stderr, "Operator: %s\n", word);
		}
	      else
		{
		  fprintf (stderr, "Word: %s\n", word);
		}
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

int 
isOperator (char *input)
{
  if (!strcmp (input, "&&") ||
      !strcmp (input, "||") ||
      !strcmp (input, "|") ||
      !strcmp (input, ";") ||
      !strcmp (input, "(") ||
      !strcmp (input, ")") ||
      !strcmp (input, "<") ||
      !strcmp (input, ">")
      )
    {
      return 1;
    }
  return 0;
}

void
testIsOperator (void)
{
  assert (isOperator ("&&") == 1);
  assert (isOperator ("||") == 1);
  assert (isOperator ("|") == 1);
  assert (isOperator ("(") == 1);
  assert (isOperator (")") == 1);
  assert (isOperator ("<") == 1);
  assert (isOperator (">") == 1);
  assert (isOperator (";") == 1);
  assert (isOperator ("word") == 0);
  assert (isOperator ("") == 0);
  assert (isOperator ("asd7f2lsd&") == 0);
  assert (isOperator ("&") == 0);
  fprintf (stderr, "All tests for 'isOperator' passed.\n");
}
