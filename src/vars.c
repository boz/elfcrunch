/*
#include"bintree.h"
*/
#include"elflib.h"
#include"vars.h"
#include"utils.h"
#include"common.h"

btree_t * var_tree = 0;

int free_var( var_t * var )
{
   list_free_list( var->refs , NULL);
   free_obj ( var->obj );
   free(var);
   return(0);
}

btree_t * get_var_tree()
{
   if(!var_tree)
      var_tree = new_btree();
   return(var_tree);
}

var_t * new_var()
{
   var_t * ret;
   ret = xcalloc( 1 , sizeof(*ret));
   ret->obj  = new_obj();
   ret->refs = new_list(NULL);
   return(ret);
}

var_t * var_at_addr( addr_t addr )
{
   var_t * ret;
   btree_t * tree;
   btree_data_t * data;
   ret = NULL;
   if(!(tree = get_var_tree()))
      error_ret("can't get tree",NULL);
   if( addr < 0 )
      error_ret("bad address",NULL);

   if(( ret = btree_find_data( tree , addr ) ))
      return(ret);

   return(NULL);
}

char * var_name_at_addr( addr_t addr )
{
   char * ret = NULL;
   var_t * var;
   if(( var = var_at_addr( addr ) ))
      if( var->obj->name && var->obj->name[0] ) 
         ret = var->obj->name;
   return(ret);
}

/*
 * if overwrite is not zero, we overwrite any old node.
 */

int add_var( var_t * var , int overwrite)
{
   bnode_t * cur;
   btree_t * tree;
   btree_data_t * data;

   if(!var||!var->obj) error_ret("null args",-1);

   addr_t addr = OBJ_GET_ADDR(var->obj);

   if(!(tree = get_var_tree()))
      error_ret("can't get var tree",-1);

   if((cur = btree_find( tree , addr ))){
      if( overwrite != 0 ) {
         free_var( cur->data.data );
         cur->data.data = var;
      }
      return(0);
   }

   data = xcalloc( 1 , sizeof(*data));

   data->key  = addr;
   data->data = var;

   if(!btree_insert( tree , data ))
      error_ret("can't add var",-1);

   return(0);
}

int gather_vars( elf_t * elf )
{
   if(!elf) error_ret("bad arg",-1);
   gather_vars_by_symtab(elf);
   gather_vars_by_dsymtab(elf);
   gather_string_vars( elf );
   return(0);
}

int gather_vars_by_symtab(elf_t * elf)
{
   Elf32_Sym * sym;
   var_t * var;
   char   * name;

   int i = 0;
   while( (sym = sym_by_type( elf , STT_OBJECT , i++ )))
   {
      if( sym->st_value != 0 ){ /* must have a vma */
         var = new_var();
         if(!(name = str_by_offset( elf , sym->st_name )))
            name = "";
         
         OBJ_SET_NAME(var->obj,name);
         OBJ_SET_ADDR(var->obj,sym->st_value);
         OBJ_SET_SIZE(var->obj,sym->st_size);
         add_var( var , 0 );
      }
   }

   i = 0;
   while( (sym = sym_by_type( elf , STT_NOTYPE , i++ )))
   {
      if( sym->st_value != 0 ){ /* must have a vma */
         var = new_var();
         if(!(name = str_by_offset( elf , sym->st_name )))
            name = "";
         
         OBJ_SET_NAME(var->obj,name);
         OBJ_SET_ADDR(var->obj,sym->st_value);
         OBJ_SET_SIZE(var->obj,sym->st_size);
         add_var( var , 0 );
      }
   }
   return(0);
}

int gather_vars_by_dsymtab( elf_t * elf )
{
   Elf32_Sym * sym;
   var_t * var;
   char   * name;

   int i = 0;
   while( (sym = dsym_by_type( elf , STT_OBJECT , i++ )))
   {
      if( sym->st_value != 0 ){ /* must have a vma */
         var = new_var();
         if(!(name = dstr_by_offset( elf , sym->st_name )))
            name = "";
         
         OBJ_SET_NAME(var->obj,name);
         OBJ_SET_ADDR(var->obj,sym->st_value);
         OBJ_SET_SIZE(var->obj,sym->st_size);
         add_var( var , 0 );
      }
   }

   i = 0;
   while( (sym = dsym_by_type( elf , STT_NOTYPE , i++ )))
   {
      if( sym->st_value != 0 ){ /* must have a vma */
         var = new_var();
         if(!(name = dstr_by_offset( elf , sym->st_name )))
            name = "";
         
         OBJ_SET_NAME(var->obj,name);
         OBJ_SET_ADDR(var->obj,sym->st_value);
         OBJ_SET_SIZE(var->obj,sym->st_size);
         add_var( var , 0 );
      }
   }

   return(0);
}

typedef struct
{
   addr_t startvm;
   size_t startoff;
} str_find_t;

#define new_str_find_t() xcalloc( 1, sizeof( str_find_t ) ) 

typedef int (*str_found_func) ( char * str , size_t offset , void * arg );


int var_add_string( char * str , size_t offset , void * arg )
{
   str_find_t * st;
   var_t * var;
   char name[16];
   addr_t addr;

   if(!arg||!str) error_ret("null arg",-1);

   st = (str_find_t*)arg;

   addr = st->startvm + offset;
   if( var_at_addr( addr ) )
      return(0);

   var = new_var();

   OBJ_SET_ADDR(var->obj, addr  );
   OBJ_SET_OFFSET( var->obj, st->startoff + offset );

   if( strlen(str) < 16 )
   {
      OBJ_SET_NAME( var->obj, str ); 
   }
   else
   {
      snprintf(name,15,"%s",str);
      OBJ_SET_NAME( var->obj , name );
   }

   if(add_var( var , 0 )<0)
      error_ret("add_var() failed",-1);

   return(0);
}


int find_strings_in_buffer( char * data , 
                            size_t size , 
                            int str_thresh,
                            str_found_func cb , 
                            void * cb_data
                          )
{
   char * last,
        * cur ;

   if(!data ||!cb ) error_ret("null arg",-1);

   last = 0;
   cur = data;

   for( cur = data ; (cur - data) < size ; cur++ )
   {
      if( isgraph(*cur) || *cur==' ' || *cur=='\n' )
      {
         if( last == 0 )
         {
            last = cur;
         }
      }
      else if( *cur != '\0' ) /* XXX: only get null terminated ascii */
      {
         last = 0;
      }
      else
      {
         if( last != 0 )
         {
            if( (cur - last) >= str_thresh )/* we have a string */
            {
               if((*cb)( last , (last - data ) , cb_data )<0)
               {
                  error_ret("problem in cb",-1);
               }
            }
            last = 0;
         }
      }
   }
   return( (cur - data) );
}


int gather_string_vars( elf_t * elf )
{
   str_find_t * st;
   char * data;
   if( ! elf )
      error_ret("null arg",-1);

   st = new_str_find_t();

   Elf32_Phdr * phdr;
   int count,
       i;

   i=0;
   while((phdr = phdr_by_type( elf , PT_LOAD , i++ ) ))
   {

      if(!(data = data_at_offset( elf , phdr->p_offset ) )){
         free(st);
         error_ret("can't get data",-1);
      }

      st->startvm  = phdr->p_vaddr;
      st->startoff = phdr->p_offset;

      if( find_strings_in_buffer(  data , phdr->p_filesz , STR_THRESH , 
                                   var_add_string , st ) < 0 )
      {
         free(st);
         error_ret("problem finding strings",-1);
      }
   }

   free(st);
   return(0);
}

int dump_one_var( btree_data_t * tdata , void * data )
{
   var_t * var = tdata->data;
   dump_one_object( var->obj );
   return(0);
}

int dump_vars()
{
   btree_t * tree;
   if(!(tree = get_var_tree()))
      error_ret("can't get tree",-1);
   btree_traverse( tree , dump_one_var , NULL , 1 );
   return(0);
}

