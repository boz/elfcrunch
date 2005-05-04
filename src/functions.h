#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include"objects.h"
#include"bintree.h"
#include"list.h"
#include"elflib.h"

#define NODE2FROM(a) ((refmap_t*)((a)->data.data))->from
#define NODE2TO(a)   ((refmap_t*)((a)->data.data))->to

typedef struct
{
   eobj_t * obj;   /* name, size, ect...         */
   list_t * calls; /* list of functions it calls */
} func_t ;

typedef struct 
{
   addr_t from; /* from where? */
   addr_t to;   /* to where?   */
} refmap_t;

int free_func( func_t * func );
btree_t * get_func_tree();
func_t * new_func();
func_t * func_at_addr( addr_t addr );
func_t * func_by_str( char * str );

func_t * func_by_name( char * name , int indx );
int func_add_call( func_t * func , addr_t call );
int func_clear_calls( func_t * func );
char * func_name_at_addr( addr_t addr );
int add_func( func_t * func , int overwrite);
int map_over_functions( btree_callback cb , void * data , int type );
char * func_get_name( func_t * func );
list_t  * get_call_list();
int add_call_ref( refmap_t * refmap );
int ref_from_cmp( list_data_t * a , list_data_t * b  );
int ref_to_cmp( list_data_t * a , list_data_t * b  );
int add_funcs_in_list( void * data , void * arg );
int gather_funcs_by_section( char * name );
int gather_funcs_by_symtab(elf_t * elf);
int sync_func_map();
int dump_functions();

#endif /* FUNCTIONS_H */

