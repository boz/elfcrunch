#ifndef COMMANDS_H
#define COMMANDS_H

/*
 * Commands
 */

typedef struct 
{
   char * str;              /* option */
   char * msg;              /* info   */
   int (*func)( char ** );  /* func to handle option */
} cmd_t ;

/*
 * Create new commands with register_command.
 * Each command should have a function to handle the input.
 * Also there is an optional func to add sub-options to 
 * the command, ex: 'show ehdr'
 */

int register_command( char * path , cmd_t * comm );

/* user input goes here */
int run_command    ( char ** cvec ); /* check all commands */

/* add the default commands */
int register_builtins();


/* struct for list.  a command like 'show' will be
 * located in the head's 'opts' list. 
 * list_insert( head->opts , show );
 *
 * 'show func' would have func in show's opt's list.
 * list_insert( show->opts , func );
 *
 * show func 
 * show data
 *
 * list_insert( show->opts , func );
 * list_insert( show_opts  , data );
 *
 * Every time the 'opts' list is traversed, one argument
 * is taken from the argument vector.
 *
 */

typedef struct _cmd_node_t
{
   struct _cmd_node_t * next; /* next node in list   */
   struct _cmd_node_t * opts; /* head of sub-options */
   cmd_t              * cmd ;
} cmd_node_t;

cmd_node_t * get_command_head();


#define COMMAND_ERROR   ((int)-1)
#define COMMAND_SUCCESS ((int)0)

#endif /* COMMANDS_H */

