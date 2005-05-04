#ifndef OBJECTS_H
#define OBJECTS_H
#include<sys/types.h>
#include<string.h>

typedef struct
{
   long   addr;
   size_t offset;
   size_t size;
   char  *name;
} eobj_t;

#define OBJ_SET_ADDR( a , b )   ( a )->addr   = ( b )
#define OBJ_SET_SIZE( a , b )   ( a )->size   = ( b )
#define OBJ_SET_OFFSET( a , b ) ( a )->offset = ( b )

#define OBJ_SET_NAME( a , b ) do{ \
   if( ( a )->name ) free( ( a )->name ); \
   ( a )->name = strdup( ( b ) ? ( b ) : ("") ); \
} while(0)

#define OBJ_GET_ADDR( a )   ( a )->addr   
#define OBJ_GET_SIZE( a )   ( a )->size   
#define OBJ_GET_OFFSET( a ) ( a )->offset 
#define OBJ_GET_NAME( a )   (( a )->name ? (a)->name : (""))

eobj_t * new_obj();
int free_obj( eobj_t * obj );
int dump_one_object( eobj_t * obj );

#endif /* OBJECTS_H */

