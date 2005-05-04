#include"objects.h"
#include"utils.h"

eobj_t * new_obj()
{
   eobj_t * obj = xcalloc( 1 , sizeof( *obj ));
   return(obj);
}

int free_obj( eobj_t * obj )
{
   if( obj ) {
      if( obj->name ) free(obj->name);
      free(obj);
   }
   return(0);
}

int dump_one_object( eobj_t * obj )
{
   if(obj){
      printf("%#-12x ",OBJ_GET_ADDR(obj));
      printf("%s"     ,OBJ_GET_NAME(obj));
      printf("\n");
   }
   return(0);
}

