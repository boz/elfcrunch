#include<stdlib.h>

#include"commands.h"
#include"common.h"

int do_help( cmd_node_t * head ,  char **argv )
{
   cmd_node_t * cur;
   int ret = 0;

   if(!head)
      error_ret("no head",COMMAND_ERROR);

   if(! *argv ) {
      if( ! head->cmd )
         error_ret("no command here",COMMAND_ERROR);

      printf("%s:\t%s\noptions:\n",head->cmd->str,head->cmd->msg);
   }

   for( cur = head->opts ; cur ; cur = cur->next ){

      if(! cur->cmd || !cur->cmd->str  )
            error_ret("bad command!",COMMAND_ERROR);

      if(!*argv)  /* print all options */
      {
         if(!cur->cmd->msg)
            error_ret("no message",COMMAND_ERROR);

         printf("%s:\t%s\n",cur->cmd->str,cur->cmd->msg);

      }
      else /* go down another level */
      {
         if( !strcmp(cur->cmd->str , argv[0] ) )
         {
            ret = do_help( cur , argv + 1 );
            break;
         }
      }
   }
   return(ret);
}

int command_help( char** argv );

cmd_t command_h = { 
   "help" , 
   "Help System",
   command_help
};

int command_help( char** argv ){
   cmd_node_t * cmds;

   if(!(cmds = get_command_head()))
      error_ret("can't get commands",COMMAND_ERROR);

   if(!*argv){ /* top-level */
      printf("%s:\t%s\noptions:\n",command_h.str,command_h.msg);
      for(cmds = cmds->opts; cmds ; cmds=cmds->next){
         if(!cmds->cmd ||!cmds->cmd->str ||!cmds->cmd->msg )
            error_ret("command has no name",COMMAND_ERROR);
         printf("%s:\t%s\n",cmds->cmd->str,cmds->cmd->msg);
      }
   }
   else
   {
      return( do_help( cmds , argv ) );
   }
}


int register_help()
{ 
   return( register_command( "/" , &command_h ) );
}

