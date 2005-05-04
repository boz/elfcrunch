#ifndef PLTMAP_H
#define PLTMAP_H
#include "utils.h"
#include "elflib.h"
#include "common.h"

typedef struct 
{
   addr_t vaddr; /* addr that gets called */
   addr_t off;   /* offset in rel_plt     */
   char * name;  /* name                  */
} plt_map_t;

#define new_plt_map() xcalloc( 1 , sizeof( plt_map_t ) )

list_t * get_pltmap();
int dump_pltmap();
int add_pltmap( list_t * pltmap , plt_map_t * map );
int crunch_pltmap();
int dump_one_pltmap( void * data , void * arg );
int dump_pltmap();
plt_map_t * pltmap_by_addr( addr_t addr );
char * pltmap_name_by_addr( addr_t addr );

#endif /* PLTMAP_H */
