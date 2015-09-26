// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <string.h>

/* Helper functions */

int 
isOperand (char *input)
// Returns 1 if input is operand

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}


/* Helper functions */

int 
isOperand (char *input)
{
  if (!strcmp (input, "&&")
      || !strcmp (input, "||")
      || !strcmp (input, "|")
      || !strcmp (input, ";")
      || !strcmp (input, "(")
      || !strcmp (input, ")")
      || !strcmp (input, "<")
      || !strcmp (input, ">")
      )
    {
      fprintf (stderr, "Encountered operand (special character).\n");
      return 1;
    }
  return 0;
}
