#ifndef REFMAP_H
#define REFMAP_H

#include"elflib.h"
#include"utils.h"
typedef struct 
{
   addr_t from; /* from where? */
   addr_t to;   /* to where?   */
} refmap_t;
#define new_refmap()  xcalloc( 1 , sizeof(refmap_t));

typedef list_t reflist_t;
#define new_reflist() new_list(NULL);

int insert_ref_map( reflist_t * reflist , refmap_t * map );
int add_ref_map( reflist_t * reflist , addr_t from , addr_t to );

int dump_reflist( void * data , void * arg );

#define NODE2FROM(a) ((refmap_t*)((a)->data.data))->from
#define NODE2TO(a)   ((refmap_t*)((a)->data.data))->to

#endif /* REFMAP_H */
