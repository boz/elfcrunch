#include"commands.h"
#include"common.h"

#include<stdlib.h>
#include<string.h>

static cmd_node_t command_head = {  0 , 0 , 0 };

cmd_node_t * get_command_head() { return &command_head; }

int register_command( char * path , cmd_t * comm ){

   if(!path || !is_good_path(path) )
      error_ret("bad arg",-1);

   cmd_node_t * head = command_head.opts;
   if( ! head ) {
      /* path should be "/" */
      head = calloc( 1 , sizeof(*head) );
      head->cmd = comm;
      command_head.opts = head;
   } else {
      char * npath = strdup(path);
      if( insert_command( npath , &command_head , comm ) < 0 ) {
         free(npath);
         error_ret("can't insert",-1);
      }
      free(npath);
   }
   return(0);
}

/* 
 * Path will be something like "/something/".
 */

int is_good_path( char * path )
{
   if( !path ||  *path != '/' )
      return (0);

   while( *++path != 0 )
      if( !( isalnum(*path) || *path == '/' ) )
         return(0);
   return(1);
}

cmd_node_t * add_comm_to_list( cmd_node_t * head , cmd_t * comm )
{
   /* XXX: probably want to deep-copy here */
   if(!comm)
      error_ret("null args",NULL);

   cmd_node_t * newnode = calloc( 1 , sizeof(*newnode));
   newnode->cmd = comm;

   if( !head )
      return(newnode);
   cmd_node_t * cur , *last;
   for( cur = head ; cur ; cur = cur->next  ) { last = cur; }
   last->next = newnode;

   return(head);
}

cmd_node_t * find_command( cmd_node_t * head , char * str )
{
   if(!str) return( NULL );
   for( ; head ; head = head->next )
      if( ! strcmp(str, head->cmd->str) )
         return(head);
   return(head);
}


int insert_command( char * path , cmd_node_t * head , cmd_t * comm )
{
   if( !comm  || ! path || ! head ) 
      error_ret("null arg",-1);


   char * nextstr;
   if( !(nextstr = strchr( path + 1 , '/' )) ) { /* add here */
      head->opts = add_comm_to_list( head->opts , comm );
   }
   else  /* it's a sub-command */
   {
      *nextstr = 0;
      cmd_node_t * nextnode ;

      /* we should have the requested child */
      if(!(nextnode = find_command( head->opts , path + 1 )))
         error_ret("request sub-command, but none available",-1);

      *nextstr = '/';
      if( insert_command( nextstr ,  nextnode , comm ) < 0 ) 
         error_ret("recursive badness",-1);
   }

   return(0);
}

int run_commands( cmd_node_t * head , char ** cvec )
{
   int ret;

   if(!head || !cvec ) error_ret("null args",-1);

   cmd_node_t * next;

   /* XXX: what if an argument to a command is also a command itself? */

   if(!(next = find_command( head->opts , cvec[0] ))) /* we're here */
   {
      if( head->cmd )
         ret = head->cmd->func( cvec );
      else
         error_ret("no command",-1);
   }
   else
      ret = run_commands( next , ++cvec );

   return(ret);
}

int run_command( char ** cvec )
{
   return( run_commands( &command_head  , cvec ) );
}

