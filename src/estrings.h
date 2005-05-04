#ifndef STRINGS_H
#define STRINGS_H
#include"objects.h"
#include"bintree.h"
#include"list.h"
#include"elflib.h"

typedef struct
{
   eobj_t * obj ;   /* name, size, ect...         */
   list_t * refs;   /* where referenced from?     */
} str_t ;

int free_str( str_t * str );
btree_t * get_str_tree();
str_t * new_str();
char * str_name_at_addr( addr_t addr );
int add_str( str_t * str , int overwrite);

int gather_strs(elf_t * elf);

int dump_strtions();
str_t * str_at_addr( addr_t addr );


#endif /* STRINGS_H */

