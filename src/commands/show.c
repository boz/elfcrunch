#include<stdlib.h>
#include"commands.h"
#include"common.h"

int command_show( char** argv ){
   if( !argv || ! *argv ) 
      error_ret("null arg", COMMAND_ERROR );
   printf("Show command\n");
   return(COMMAND_SUCCESS);
}

cmd_t command = { 
   "show" , 
   "Show info about some data",
   command_show
};

int register_show()
{ 
   return( register_command( "/" , &command ) );
}


