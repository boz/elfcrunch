
#include"commands.h"
#include"common.h"
#include<stdlib.h>

int command_test1( char** argv ){
   if( !argv ) error_ret("null arg", COMMAND_ERROR );
   printf("inside test1\n");

   while( *argv != 0 ){
      printf("%s\n",*argv++);
   }
   return(COMMAND_SUCCESS);
}


int command_test2( char** argv ){
   if( !argv ) error_ret("null arg", COMMAND_ERROR );
   printf("inside test2\n");
   while( *argv != 0 ){
      printf("%s\n",*argv++);
   }
   return(COMMAND_SUCCESS);
}

cmd_t command1 = { 
   "test1" , 
   "Test 1",
   command_test1
};

cmd_t command2 = { 
   "test2" , 
   "Test 2",
   command_test2
};

int register_test1()
{ 
   return( register_command( "/" , &command1 ) );
}

int register_test2()
{ 
   return ( register_command( "/test1/",&command2 ) );
   /* return( register_command( "/" , &command2 ) ); */

}

