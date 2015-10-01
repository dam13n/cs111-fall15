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
  TOKEN_COMMENT,
  TOKEN_SEPARATOR
} token_type;

typedef struct token
{
  enum token_type type;
  char *word;
} token;

typedef token *token_t;




// token_t get_token (char *word, token_type type, int *is_word);
// void print_token (token_t token);
// void clean_up_token (token_t * token, char **word, FILE ** stream,
// 		     size_t * size);


/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
struct command_stream
{
  // struct command_t *commands;
};

token_type get_token_type (char *word);
token_t get_token (char *word, token_type type);
void print_token (token_t token);
void clean_up (token_t token, FILE *stream);

// Used now for testing
void process_print (char *word, token_type type, FILE *stream);

void stream_write (FILE *stream, int c);

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
	FILE *stream;
	size_t size;
	char *buffer;
	stream = open_memstream (&buffer, &size);

	// // flags
	// int comment_flag = 0;
	// int pc_set_flag = 0;
	// int and_or_flag = 0;
	// int special_flag = 0;

	// currently read character
	int c;
	// // previously read character
	// int pc;

	while ((c = (*get_next_byte) (get_next_byte_argument)) != EOF)
	{
		fprintf(stream, "%c", c);
		fflush (stream);
	}
	fprintf(stderr, "%s\n", buffer);
		// switch (c)
		// {
		// 	case '\n':
		// 	{
		// 		if (comment_flag)
		// 		{
		// 			comment_flag = 0;
		// 			process_print (word, TOKEN_COMMENT, stream);
		// 		}
		// 		else if (pc_set_flag)
		// 		{
		// 			switch (pc)
		// 			{
		// 				case '<':
		// 				case '>':
		// 				case '(':
		// 				case ')':
		// 				case '|':
		// 					break;

		// 				case '&':
		// 				{
		// 					if (!and_or_flag)
		// 					{
		// 						process_print (word, get_token_type (word), stream);
		// 					}
		// 					break;
		// 				}

		// 				default:
		// 				{
		// 					process_print (word, get_token_type (word), stream);
		// 					break;
		// 				}
		// 			}
		// 		}
		// 		break;
		// 	}

		// 	case ' ':
		// 	{
		// 		if (comment_flag)
		// 		{
		// 			stream_write (stream, c);
		// 		}
		// 		else if (pc_set_flag)
		// 		{
		// 			switch (pc)
		// 			{
		// 				case '<':
		// 				case '>':
		// 				case '(':
		// 				case ')':
		// 				case '|':
		// 				case '\n':
		// 					break;

		// 				case '&':
		// 				{
		// 					if (!and_or_flag)
		// 					{
		// 						process_print (word, get_token_type (word), stream);
		// 					}
		// 					break;
		// 				}

		// 				default:
		// 				{
		// 					process_print (word, get_token_type (word), stream);
		// 					break;
		// 				}
		// 			}
		// 		}
		// 		break;
		// 	}

		// 	case '|':
		// 	{
		// 		if (comment_flag)
		// 		{
		// 			stream_write (stream, c);
		// 		}
		// 		else if (pc_set_flag)
		// 		{
		// 			switch (pc)
		// 			{
		// 				case '|':
		// 				{
		// 					if (and_or_flag)
		// 					{
		// 						// report error
		// 						exit;
		// 					}
		// 					else
		// 					{
		// 						stream_write (stream, special_char);
		// 						stream_write (stream, c);
		// 						process_print (word, get_token_type (word), stream);
		// 						and_or_flag = 1;
		// 					}
		// 					break;
		// 				}

		// 				case '&':
		// 				{
		// 					if (and_or_flag)
		// 					{
		// 						stream_write(stream, c);
		// 						and_or_flag = 0;
		// 					}
		// 					else
		// 					{
		// 						process_print (word, get_token_type (word), stream);
		// 						stream_write (stream, c);
		// 					}
		// 				}

		// 				case ' ':
		// 				case '\n':
		// 				case '<':
		// 				case '>':
		// 				case '(':
		// 				case ')':
		// 				{
		// 					stream_write (stream, c);
		// 					break;
		// 				}

		// 				default:
		// 				{
		// 					process_print (word, get_token_type (word), stream);
		// 					stream_write (stream, c);
		// 					break;
		// 				}
		// 			}
		// 		}
		// 		break;
		// 	}

		// 	case '&':
		// 	{
		// 		if (comment_flag)
		// 		{
		// 			stream_write (stream, c);
		// 		}
		// 		else if (pc_set_flag)
		// 		{
		// 			switch (pc)
		// 			{
		// 				case '|':
		// 				{
		// 					if (and_or_flag)
		// 					{
		// 						stream_write (stream, c);
		// 						and_or_flag = 0;
		// 					}
		// 					else
		// 					{
		// 						process_print (word, get_token_type (word), stream);
		// 						stream_write (stream, c);
		// 					}
		// 					break;
		// 				}

		// 				case '&':
		// 				{
		// 					if (and_or_flag)
		// 					{
		// 						// report error
		// 						exit;
		// 					}
		// 					else
		// 					{

		// 						special_flag = 0;
		// 						and_or_flag = 1;
		// 					}
		// 				}

		// 				default:
		// 				{

		// 				}
		// 			}
		// 		}
		// 		break;
		// 	}

		// 	case '<':
		// 	{
		// 		break;
		// 	}

		// 	case '>':
		// 	{
		// 		break;
		// 	}

		// 	case '(':
		// 	{
		// 		break;
		// 	}

		// 	case ')':
		// 	{
		// 		break;
		// 	}

		// 	case '#':
		// 	{
		// 		comment_flag = 1;
		// 		break;
		// 	}

		// 	default:
		// 	{
		// 		break;
		// 	}
		// }

		// // set pc to be equal to c
		// pc = c;
		// // set pc_set_flag to 1
		// pc_set_flag = 1;


  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  return 0;
}

// Helper functions

// void 
// stream_write (FILE *stream, int c)
// {
// 	fprintf (stream, "%c", c);
// 	fflush (stream);
// }

// token_t
// get_token (char *word, token_type)
// {

// }

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



// token_t
// get_token (char *word, token_type type, int *is_word)
// {
//   if (word == NULL)
//     return NULL;
//   if (strlen (word) == 0 && type != TOKEN_SEPARATOR)
//     return NULL;

//   token_t token = (token_t) malloc (sizeof (struct token));
//   (*token).type = type;
//   (*token).word = (char *) malloc (strlen (word) + 1);
//   strcpy ((*token).word, word);

//   if (type == TOKEN_WORD)
//     *is_word = 1;
//   else
//     *is_word = 0;

//   return token;
// }

// void
// print_token (token_t token)
// {
//   if (token == NULL)
//     return;
//   token_type type = (*token).type;
//   if (type == TOKEN_SEPARATOR)
//     fprintf (stderr, "type: %d\n", (*token).type);
//   else
//     fprintf (stderr, "type: %d  \t  word: %s\n", (*token).type,
// 	     (*token).word);
// }

// void
// clean_up_token (token_t * token, char **word, FILE ** stream, size_t * size)
// {
//   if (token == NULL)
//     return;
//   if (word == NULL)
//     return;
//   if (stream == NULL)
//     return;
//   // fprintf (stderr, "1\n");
//   free ((**token).word);
//   free (*token);
//   // fprintf (stderr, "2\n");
//   // fclose (*stream);
//   // fprintf (stderr, "3\n");
//   // free (*word);
//   // fprintf (stderr, "4\n");
//   // free (size);
//   // fprintf (stderr, "5\n");
//   //*stream = open_memstream (word, size);
//   // fprintf (stderr, "6\n");
//   fseeko (*stream, 0, SEEK_SET);
//   // fprintf(stderr, "WORD: %s\n", *word);
// }

// FILE *stream;
//   size_t size;
//   char *word;
//   stream = open_memstream (&word, &size);

//   int is_special = 0;
//   int is_new_line = 0;
//   int is_word = 0;
//   int is_comment = 0;
//   int special_char;

//   int c;
//   while ((c = (*get_next_byte) (get_next_byte_argument)) != EOF)
//     {
//       // fprintf (stderr, "byte: %c\n", c);
//       if (c == '#')
// 	{
// 	  is_comment = 1;
// 	  fprintf (stream, "%c", c);
// 	  fflush (stream);
// 	}
//       else if (!is_comment || c == '\n')
// 	{
// 	  if (c != '\n' && c != ' ')
// 	    {
// 	      if (is_special)
// 		{
// 		  if (special_char == c)
// 		    {
// 		      token_t token = get_token (word, get_token_type (word),
// 						 &is_word);
// 		      print_token (token);
// 		      clean_up_token (&token, &word, &stream, &size);

// 		      fprintf (stream, "%c%c", special_char, c);
// 		      fflush (stream);

// 		      token =
// 			get_token (word, get_token_type (word), &is_word);
// 		      print_token (token);
// 		      clean_up_token (&token, &word, &stream, &size);

// 		      is_special = 0;
// 		    }
// 		  else
// 		    {
// 		      if (special_char == '|')
// 			{
// 			  token_t token =
// 			    get_token (word, get_token_type (word),
// 				       &is_word);
// 			  print_token (token);
// 			  clean_up_token (&token, &word, &stream, &size);

// 			  fprintf (stream, "%c", special_char);
// 			  fflush (stream);

// 			  token =
// 			    get_token (word, get_token_type (word), &is_word);
// 			  print_token (token);
// 			  clean_up_token (&token, &word, &stream, &size);
// 			}
// 		      else
// 			{
// 			  fprintf (stream, "%c%c", special_char, c);
// 			  fflush (stream);
// 			}

// 		      is_special = 0;
// 		    }
// 		}
// 	      else if (c == '&' || c == '|')
// 		{
// 		  is_special = 1;
// 		  special_char = c;
// 		}
// 	      else if (c == '<' ||
// 		       c == '>' || c == '(' || c == ')' || c == ';')
// 		{
// 		  token_t token =
// 		    get_token (word, get_token_type (word), &is_word);
// 		  print_token (token);
// 		  clean_up_token (&token, &word, &stream, &size);

// 		  fprintf (stream, "%c", c);
// 		  fflush (stream);

// 		  token = get_token (word, get_token_type (word), &is_word);
// 		  print_token (token);
// 		  clean_up_token (&token, &word, &stream, &size);
// 		}
// 	      else
// 		{
// 		  fprintf (stream, "%c", c);
// 		  fflush (stream);
// 		}
// 	      is_new_line = 0;
// 	    }

// 	  else if (c == '\n')
// 	    {
// 	      if (is_comment)
// 		{
// 		  token_t token = get_token (word, TOKEN_COMMENT, &is_word);
// 		  print_token (token);
// 		  clean_up_token (&token, &word, &stream, &size);
// 		  is_comment = 0;
// 		  fprintf (stderr, "Got here\n");
// 		}
// 	      else if (is_new_line)
// 		{
// 		  if (is_special)
// 		    {
// 		      token_t token = get_token (word, get_token_type (word),
// 						 &is_word);
// 		      print_token (token);
// 		      clean_up_token (&token, &word, &stream, &size);

// 		      fprintf (stream, "%c", special_char);
// 		      fflush (stream);
// 		      is_special = 0;
// 		    }

// 		  token_t token =
// 		    get_token (word, get_token_type (word), &is_word);
// 		  print_token (token);
// 		  clean_up_token (&token, &word, &stream, &size);

// 		  if (is_word)
// 		    {
// 		      token = get_token ("", TOKEN_SEPARATOR, &is_word);
// 		      print_token (token);
// 		      clean_up_token (&token, &word, &stream, &size);

// 		      is_new_line = 0;
// 		    }
// 		  else
// 		    {

// 		    }
// 		}
// 	      else
// 		{
// 		  is_new_line = 1;
// 		}
// 	    }

// 	  else
// 	    {
// 	      if (is_special)
// 		{
// 		  fprintf (stream, "%c", special_char);
// 		  fflush (stream);
// 		  is_special = 0;
// 		}
// 	      if (strlen (word) > 0)
// 		{
// 		  token_t token =
// 		    get_token (word, get_token_type (word), &is_word);
// 		  print_token (token);
// 		  clean_up_token (&token, &word, &stream, &size);
// 		}
// 	      else
// 		{
// 		  free (word);
// 		  stream = open_memstream (&word, &size);
// 		}
// 	      is_new_line = 0;
// 	    }
// 	}

//       else
// 	{
// 	  fprintf (stream, "%c", c);
// 	  fflush (stream);
// 	}
//     }

//   token_t token = get_token (word, get_token_type (word), &is_word);
//   print_token (token);
//   clean_up_token (&token, &word, &stream, &size);

//   free (word);
//   fclose (stream);