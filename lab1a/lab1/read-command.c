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
struct command_stream
{

};

int is_valid (int c);
int is_special (int c);

char *get_stream_from_input (int (*get_next_byte) (void *), 
	void *get_next_byte_argument);

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
	char *stream = get_stream_from_input (get_next_byte, get_next_byte_argument);
	fprintf(stderr, "%s\n", stream);
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  return 0;
}

/*************************************/
/************** helpers **************/
/*************************************/

char *get_stream_from_input (int (*get_next_byte) (void *), 
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
	free (buffer);

	return script;
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
