/*
#include"bintree.h"
*/
#include"elflib.h"
#include"functions.h"
#include"utils.h"
#include"common.h"
#include"edisasm.h"
#include"target.h"

btree_t * func_tree = 0;

int free_func( func_t * func )
{
   list_free_list( func->calls , (free_cb)free );
   free_obj ( func->obj );
   free(func);
   return(0);
}


btree_t * get_func_tree()
{
   if(!func_tree)
      func_tree = new_btree();
   return(func_tree);
}

func_t * new_func()
{
   func_t * ret;
   ret = xcalloc( 1 , sizeof(*ret));
   ret->obj = new_obj();
   return(ret);
}

func_t * func_at_addr( addr_t addr )
{
   func_t * ret;
   btree_t * tree;
   btree_data_t * data;
   ret = NULL;
   if(!(tree = get_func_tree()))
      error_ret("can't get tree",NULL);
   if( addr < 0 )
      error_ret("bad address",NULL);

   if(( data = btree_find_data( tree , addr ) ))
      ret = (func_t*)data;
   return(ret);
}


typedef struct
{
   func_t * func;
   char * name;
   int indx;
} find_func_t;

int func_has_name( btree_data_t * data  , find_func_t * ff )
{
   func_t * func; 
   if(!data||!ff) error_ret("null args",0);
   func = data->data;

   if(!(strcmp(OBJ_GET_NAME(func->obj),ff->name)))
   {
      if( ff->indx-- >= 0 )
      {
         ff->func = func;
         return(-1);
      }
   }
   return(0);
}

func_t * func_by_name( char * name , int indx )
{
   find_func_t * ff;
   func_t * func;
   if(!name) error_ret("null arg",NULL);
   ff = xcalloc(1, sizeof(*ff));

   ff->name = name;
   ff->indx = indx;

   map_over_functions((btree_callback)func_has_name , ff , 0 );
   func = ff->func;

   free(ff);
   return(func);
}

func_t * func_by_str( char * str )
{
   func_t * func = NULL;
   if(!str) error_ret("null arg",NULL);

   if( ! strncmp( str , "0x" , 2 ) )
   {
      addr_t addr;
      sscanf( str , "0x%x" , & addr );
      if((func = func_at_addr( addr )))
         return(func);
   }
   if(!(func = func_by_name( str , 0 )))
      error_ret("can't find function",NULL);
   return(func);
}

char * func_get_name( func_t * func )
{
   static char buf[16];
   if( !func ||  !func->obj ) error_ret("null arg",NULL);
   if( func->obj->name && func->obj->name[0] ) 
      return( func->obj->name );
   snprintf(buf,15,"%x",OBJ_GET_ADDR(func->obj));
   return(buf);
}

int func_add_call( func_t * func , addr_t call )
{
   list_data_t * data;
   if(!func||call<=0) error_ret("bad args",-1);

   if(!(func->calls))
      func->calls = new_list(NULL);

   if(!(data = list_retrieve_data( func->calls , call ))){
      data = list_new_data();
      data->key   = call;
      data->data  = (void*)call;
      list_insert( func->calls , data );
   }
   return(0);
}

int func_clear_calls( func_t * func )
{
   if(!func) error_ret("null arg",-1);
   if( func->calls ){
      list_free_list( func->calls , NULL );
      func->calls = NULL;
   }
   return(0);
}

char * func_name_at_addr( addr_t addr )
{
   char * ret = NULL;
   func_t * func;
   if(( func = func_at_addr( addr ) ))
      if( func->obj->name && func->obj->name[0] ) 
         ret = func->obj->name;
   return(ret);
}

/*
 * if overwrite is not zero, we overwrite any old node.
 */

int add_func( func_t * func , int overwrite)
{
   bnode_t * cur;
   btree_t * tree;
   btree_data_t * data;

   if(!func||!func->obj) error_ret("null args",-1);

   addr_t addr = OBJ_GET_ADDR(func->obj);

   if(!(tree = get_func_tree()))
      error_ret("can't get func tree",-1);

   if((cur = btree_find( tree , addr ))){
      if( overwrite != 0 ) {
         free_func( cur->data.data );
         cur->data.data = func;
      }
      return(0);
   }

   data = xcalloc( 1 , sizeof(*data));

   data->key  = addr;
   data->data = func;

   if(!btree_insert( tree , data ))
      error_ret("can't add func",-1);

   return(0);
}

int map_over_functions( btree_callback cb , void * data , int type )
{
   btree_t * tree;
   if(!cb) error_ret("null args",-1);

   if(!(tree = get_func_tree()))
      error_ret("can't get tree",-1);
   return( btree_traverse( tree , cb , data , type ) );
}


typedef struct
{
   addr_t lb;     /* lowest address we want */
   addr_t ub;     /* upper bound */
   list_node_t * curfrom;
   func_t      * last_func;
} func_sync_t;

#define new_func_sync() xcalloc(1,sizeof(func_sync_t));

list_t  * call_list = 0;

list_t  * get_call_list()
{
   if(!call_list)
      call_list = new_list( (list_cmp_func)ref_from_cmp );
   return(call_list);
}

int add_call_ref( refmap_t * refmap )
{
   list_t * list;
   list_data_t * data;

   list       = get_call_list();
   data       = list_new_data();
   data->data = refmap;
   data->key  = refmap->from;

   if(list_insert( list , data )<0)
      error_ret("problem inserting",-1);

   return(0);
}

int ref_from_cmp( list_data_t * a , list_data_t * b  )
{
   refmap_t *ra,*rb;
   if(!((ra = a->data)&&(rb = b->data)))
      error_ret("bad args",0);
   return( ra->from - rb->from );
}

int ref_to_cmp( list_data_t * a , list_data_t * b  )
{
   refmap_t *ra,*rb;
   if(!((ra = a->data)&&(rb = b->data)))
      error_ret("bad args",0);
   return( ra->to - rb->to );
}

int map_call_refs( x86_insn_t * insn , void * arg )
{
   addr_t dst;
   refmap_t * map;

   if( ( insn->type != insn_call ) &&
       ( insn->type != insn_callcc ) )
      return(0);

   if((dst = op_resolve( &insn->operands[op_dest] , insn ))<=0)
      return(0);

   map = xcalloc( 1 , sizeof(*map));
   map->from = insn->addr;
   map->to   = dst;

   if( add_call_ref(map) < 0 )
      error_ret("can't add ref",-1);

   return(0);
}


int do_sync_func_map( btree_data_t * data , void * arg )
{
   func_t * func;
   func_sync_t * sync;

   if(!(func = data->data)||!(sync=(func_sync_t*)arg))
      error_ret("null arg",-1);
   
   func_clear_calls( func );

   if(!( sync->last_func ))
   {
      sync->last_func = func;
      return(0);
   }

   while( (sync->curfrom) && 
          (NODE2FROM(sync->curfrom)  < OBJ_GET_ADDR(func->obj)) )
   {
      func_add_call( sync->last_func , NODE2TO(sync->curfrom) );
      sync->curfrom = sync->curfrom->next;
   }

   OBJ_SET_SIZE( sync->last_func->obj , 
                 OBJ_GET_ADDR(func->obj)-OBJ_GET_ADDR(sync->last_func->obj));

   sync->last_func = func;

   return(0);
}

int add_funcs_in_list( void * data , void * arg )
{
   refmap_t * ref;
   func_t * func;

   if(!(ref = (refmap_t*)data))
      error_ret("bad args",-1);

   if(!(func = func_at_addr( ref->to )))
   {
      char name[16];
      func = new_func();
      snprintf(name,15,"%x",ref->to );
      OBJ_SET_ADDR(func->obj , ref->to);
      OBJ_SET_ADDR(func->obj , ref->to);
      OBJ_SET_NAME(func->obj , name   );
      if(add_func(func,1)<0)
         error_ret("can't add func",-1);
   }
   return(0);
}

/* try to get all of the functions in a section */
int gather_funcs_by_section( char * name )
{
   Elf32_Shdr * shdr;
   elf_t * elf;
   list_t * list;
   if(!name) name = ".text";

   if(!(elf = get_elf()))
      error_ret("can't get elf",-1);

   if(!(shdr = section_by_name(elf,name)))
      error_ret("no section",-1);

   /* find all of the calls in the range */
   if( edisasm_addr_range( elf , shdr->sh_addr ,
                           shdr->sh_addr + shdr->sh_size ,
                           (edis_callback)map_call_refs ,
                           NULL ) <= 0)

      error_ret("problem disassembling",-1);

   return(0);
}


int gather_funcs_by_symtab(elf_t * elf)
{
   Elf32_Sym * sym;
   func_t * func;
   char   * name;

   int i = 0;
   while( (sym = sym_by_type( elf , STT_FUNC , i++ )))
   {
      if( sym->st_value != 0 ){ /* must have a vma */
         func = new_func();
         if(!(name = str_by_offset( elf , sym->st_name )))
            name = "";
         
         OBJ_SET_NAME(func->obj,name);
         OBJ_SET_ADDR(func->obj,sym->st_value);
         OBJ_SET_SIZE(func->obj,sym->st_size);
         add_func( func , 1 );
      }
   }
   return(0);
}


/*
 * Sync-up the function map with the ref list.  Do this right before
 * you print or whatever.
 */
int sync_func_map()
{
   list_t * list;
   func_sync_t * sync;

   list = get_call_list();

   /* make sure all of the functions are in the map */
   list_map_over_data( list , add_funcs_in_list , NULL );

   sync = xcalloc(1,sizeof(*sync));
   sync->curfrom = list_root_node(list);
   sync->last_func = NULL;

   /* determine size,calls, ect... */ 
   map_over_functions( do_sync_func_map , sync , 1 );

   free(sync);
   return(0);
}

int dump_one_func( btree_data_t * tdata , void * data )
{
   func_t * func = tdata->data;
   dump_one_object( func->obj );
   return(0);
}

int dump_functions()
{
   map_over_functions( dump_one_func , NULL , 1 );
   return(0);
}

