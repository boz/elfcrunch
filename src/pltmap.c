#include"pltmap.h"
#include"target.h"
#include"functions.h"
#include<string.h>

list_t * pltmap_list = NULL;

list_t * get_pltmap()
{
   if(!pltmap_list)
      pltmap_list = new_list(NULL);
   return(pltmap_list);
}

int add_pltmap( list_t * pltmap , plt_map_t * map )
{
   list_data_t * data;
   if(!map||!pltmap) error_ret("null arg",-1);
   data = list_new_data();
   data->key = map->vaddr;
   data->data = map;
   list_insert( pltmap , data);
   return(0);
}
int do_add_pltmap_funcs( void * data , void * arg )
{
   plt_map_t * map;
   func_t * func;

   if(!(map = data))
      error_ret("no map",-1);

   if(!(func = func_at_addr( map->vaddr )))
   {
      func = new_func();
      OBJ_SET_ADDR(func->obj,map->vaddr);
      add_func( func , 1 ) ;
   }
   OBJ_SET_NAME(func->obj,map->name);

   return(0);
}


int add_pltmap_funcs(list_t * list)
{
   if(!list)
      error_ret("null list",-1);

   list_map_over_data( list , (list_callback)do_add_pltmap_funcs , NULL );

   return(0);
}

int crunch_pltmap()
{
   list_t * list;
   elf_t  * elf;
   addr_t curvma;
   char * data;
   size_t nread,
          tread = 0;

   if(!(list = get_pltmap()))
      error_ret("can't get plt_map()",-1);

   if((curvma = get_plt(NULL))<=0)
      error_ret("can't get plt",-1);

   if(!(elf = get_elf()))
      error_ret("can't get elf",-1);

   if(!(data=data_at_addr(elf,curvma)))
      error_ret("can't get data",-1);

   if((nread = crunch_plt_header( data , curvma ))<=0)
      error_ret("bad plt header",-1);
   tread  += nread;

   int reloff;
   while((nread = crunch_plt_entry( data + tread, curvma + tread , 
                                    &reloff , NULL ))>0)
   {
      plt_map_t * map;
      Elf32_Rel * rel_plt;
      char      * str;
      map = new_plt_map();

      if(!(rel_plt=get_rel_plt(elf)))
         error_ret("can't get reloc table",-1);

      map->vaddr = curvma + tread;
      map->off   = reloff;

      rel_plt += (reloff/sizeof(*rel_plt));

      str = dsym_name_by_index( elf , ELF32_R_SYM(rel_plt->r_info));

      map->name = strdup( str ? str : "" );

      add_pltmap( list , map );

      tread  += nread;
   }
   add_pltmap_funcs(list);
   return(tread);
}

int dump_one_pltmap( void * data , void * arg )
{
   plt_map_t * map;
   if(!data) error_ret("bad args",-1);

   map = (plt_map_t*)data;
   printf("%#-15x" , map->vaddr);
   printf("%#-10x" , map->off  );
   printf("%s\n"   , map->name );
   return(0);
}

int dump_pltmap()
{
   list_t * list;
   if(!(list = get_pltmap()))
      error_ret("can't get pltmap",-1);
   list_map_over_data( list , dump_one_pltmap , NULL );
   return(0);
}

plt_map_t * pltmap_by_addr( addr_t addr )
{
   list_t * list;
   plt_map_t * map;
   if( addr <= 0 )
      error_ret("bad arg",NULL);
   if(!(list = get_pltmap()))
      error_ret("can't get pltmap",NULL);
   if((map = (plt_map_t*)list_retrieve_data( list , addr )))
      return(map);
   return(NULL);
}

char * pltmap_name_by_addr( addr_t addr )
{
   plt_map_t * map;
   if( addr<= 0 ) error_ret("bad args",NULL);
   if(!(map = pltmap_by_addr( addr )))
      return(NULL);
   return (map->name);
}

