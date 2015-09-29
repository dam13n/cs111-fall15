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
    TOKEN_REDIR_OUTPUT,
    TOKEN_SEPARATOR
  } token_type;

struct token
{
  enum token_type type;
  char *word;
};

typedef struct token *token_t;

token_type get_token_type (char *word);
token_t get_token_t_from_word (char *word);

void token_print (char **word, size_t *size, FILE **stream);


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

  int is_special = 0;
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
		  token_print (&word, &size, &stream);
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
	  else if (c == '<' || 
		   c == '>' ||
		   c == '(' ||
		   c == ')' ||
		   c == ';')
	    {
	      token_print (&word, &size, &stream);

	      fprintf (stream, "%c", c);
	      fflush (stream);

	      token_print (&word, &size, &stream);
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
	      token_print (&word, &size, &stream);
	    }
	  else
	    {
	      free (word);
	      stream = open_memstream (&word, &size);
	    }
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

void token_print (char **word, size_t *size, FILE **stream)
{
  if (word == NULL)
    return;
  if (strlen (*word) == 0)
    return;
  token_t token = get_token_t_from_word (*word);
  fprintf (stderr, "type: %d, word: %s\n", (*token).type, (*token).word);
  free (token);
  free (*word);
  *stream = open_memstream (word, size);
}
