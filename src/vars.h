#ifndef VARS_H
#define VARS_H
#include"objects.h"
#include"bintree.h"
#include"list.h"
#include"elflib.h"

#define STR_THRESH 3

typedef struct
{
   eobj_t * obj;   /* name, size, ect...            */
   list_t * refs;  /* where are we referenced from? */
} var_t ;

int free_var( var_t * var );
btree_t * get_var_tree();
var_t * new_var();
char * var_name_at_addr( addr_t addr );
int add_var( var_t * var , int overwrite);
int gather_vars_by_symtab(elf_t * elf);
int dump_vars();
var_t * var_at_addr( addr_t addr );
int gather_string_vars( elf_t * elf );

#endif /* VARS_H */

