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

int var_add_string( addr_t vma , size_t off , char * str )
{
   var_t * var;
   char name[16];
   var = new_var();
   OBJ_SET_ADDR(var->obj,vma);
   OBJ_SET_OFFSET( var->obj, off );

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


int gather_string_vars( elf_t * elf )
{
   if( ! elf )
      error_ret("null arg",-1);

   Elf32_Phdr * phdr;
   int count,
       i;

   i=0;

   elfstr_t * strings = NULL;
   size_t    strcnt   = 0;
   size_t    strsz    = 0;

   while((phdr = phdr_by_type( elf , PT_LOAD , i++ ) ))
   {
      char * data,
           * last,
           * cur ;
      addr_t vma;
      size_t size;
      offset_t off;

      vma  = phdr->p_vaddr;
      off  = phdr->p_offset;
      size = phdr->p_filesz;

      fprintf(stderr,"new section:\n");
      fprintf(stderr,"size=%x\n",size);
      fprintf(stderr,"off=%x\n",off);
      fprintf(stderr,"vma=%x\n",vma);
      fprintf(stderr,"end section\n");

      if(!(data = data_at_offset( elf , off ) ) )
         error_ret("can't get data",-1);


      last = 0;
      cur = data;

      for(cur=data; (cur - data) < size ;cur++)
      {
         if( isgraph(*cur) || *cur==' ' || *cur=='\n' )
         {
            if( last == 0 )
               last = cur;

         }
         else if( *cur != '\0' ) /* XXX: only get null terminated ascii */
         {
            last = 0;
         }
         else
         {
            if( last != 0 )
            {
               if( (cur - last) >= STR_THRESH )/* we have a string */
               {
                  var_t * var;
                  if(!(var = var_at_addr( vma + (last-data) )))
                  {
                     var_add_string( vma+(last-data),off+(last-data), last );
                  }
                  last=0;
               }
               else
               {
                  last = 0;
               }
            }
         }
      }
   }

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

