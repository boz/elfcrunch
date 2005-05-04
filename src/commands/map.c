#include<stdlib.h>
#include"commands.h"
#include"common.h"
#include"functions.h"

/*
 * Map a function.  If there's 1 option, start there, if there's 2, 
 * use as depth
 */

int command_map( char** argv ){

   char * target;
   func_t * func;
   addr_t addr = 0;
   int depth = -1;

   if(! argv )  error_ret("null arg",-1);
   
   target = *argv ? *argv : "main";

   if(!(func = func_by_str( target  )))
      error_ret("can't find function", COMMAND_ERROR  )
   addr = OBJ_GET_ADDR(func->obj);

   if( argv[1] )
      if(!(depth = atoi( argv[1] )))
         depth = -1;


   dump_dot_map( addr , depth );

   return(COMMAND_SUCCESS);
}


cmd_t map_command = { 
   "map" , 
   "make a map of the calls",
   command_map
};

int register_map()
{ 
   return( register_command( "/" , &map_command ) );
}


