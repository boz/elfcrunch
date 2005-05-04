#include"commands.h"
#include"common.h"
#include<stdlib.h>

int command_set( char** argv ){
   if( !argv ) error_ret("null arg", COMMAND_ERROR );

   printf("set command\n");
   return(COMMAND_SUCCESS);
}

static cmd_t command = { 
   "set" , 
   "Set some data",
   command_set
};

int register_set()
{ 
   return( register_command( "/" , &command ) );
}

