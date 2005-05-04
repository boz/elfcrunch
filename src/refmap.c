#include"refmap.h"
#include"common.h"
#include"utils.h"

int insert_ref_map( reflist_t * reflist , refmap_t * map )
{
   list_data_t * data;

   if(!reflist ||!map)
      error_ret("null args",-1);

   data = list_new_data();

   data->data = map;
   data->key  = map->from;
   if(list_insert( reflist , data )<0)
   {
      free(data);
      error_ret("can't insert",-1);
   }
   return(0);
}

int add_ref_map( reflist_t * reflist , addr_t from , addr_t to )
{
   if(!reflist) error_ret("null arg",-1);

   refmap_t * map;
   map = new_refmap();
   map->from = from;
   map->to = to;
   if(insert_ref_map( reflist , map )<0)
   {
      free(map);
      error_ret("can't insert map",-1);
   }
   return(0);
}
int dump_reflist( void * data , void * arg )
{
   refmap_t * map;
   map = (refmap_t*)data;
   fprintf(stderr,"%x -> %x\n",map->from, map->to);
   return(0);
}

