#include"functions.h"
#include"utils.h"
#include"common.h"

typedef int (*func_map_cb) ( func_t * func , void * data );

int print_func_map( func_t * func , 
                    list_t * stoplist , 
                    int depth ,
                    int space )
{
   int i;
   list_data_t * data;

   if(!func ||  depth == 0 )
      return 0;

   if(!stoplist)
      error_ret("no stoplist!",-1);

   for( i = 0 ; i < space ; i++ )
      putchar(' ');

   if(list_retrieve_node( stoplist , OBJ_GET_ADDR(func->obj) ) )
      return(0);

   printf("%s\n",addr2str(OBJ_GET_ADDR(func->obj)));
   
   data       = list_new_data();
   data->key  = OBJ_GET_ADDR(func->obj);
   list_insert( stoplist , data );

   if( func->calls )
   {
      list_node_t * cur;
      for( cur = list_root_node(func->calls) ; cur ; cur = cur->next )
      {
         func_t * nfunc;

         if(!(nfunc = func_at_addr( LIST_NODE_KEY(cur) )))
            error_ret("no func here!",-1);

         
         print_func_map( nfunc , stoplist , depth-1 , space+2 );

      }
   }

   return(0);
}
      

int dump_func_map( addr_t addr , int depth )
{
   func_t * func;
   list_t * stoplist;

   if(!(func = func_at_addr( addr ) ))
      error_ret("no function there",-1);

   stoplist = new_list(NULL);

   print_func_map( func , stoplist , depth , 0 );

   list_free_list( stoplist , NULL );

   return(0);
}

/* XXX: calling for attention.... */
char * addr2objstr( addr_t );

char * addr2dotstr( addr_t addr )
{
   static char retstr[BUFSIZ];
   char * tmp;
   if( (tmp = addr2objstr( addr )) && tmp[0] )
      snprintf( retstr , sizeof(retstr) - 1 , "%s" , tmp );
   else
      snprintf( retstr , sizeof(retstr) - 1 , "%x" , addr );
   return(retstr);
}

int dump_func_nodes( func_t * func , 
                     list_t * stoplist , 
                     int depth )
{
   int i;
   list_data_t * data;

   if(!func ||  depth == 0 )
      return 0;

   if(!stoplist)
      error_ret("no stoplist!",-1);

   if(list_retrieve_node( stoplist , OBJ_GET_ADDR(func->obj) ) )
      return(0);


   printf("f%s [ shape=%s label=\"%s\"]\n",addr2dotstr(OBJ_GET_ADDR(func->obj)),
                                        "box",
                                       addr2dotstr(OBJ_GET_ADDR(func->obj)));
   /*
   printf("%s\n",addr2str(OBJ_GET_ADDR(func->obj)));
   */
   
   data       = list_new_data();
   data->key  = OBJ_GET_ADDR(func->obj);
   list_insert( stoplist , data );

   if( func->calls )
   {
      list_node_t * cur;
      for( cur = list_root_node(func->calls) ; cur ; cur = cur->next )
      {
         func_t * nfunc;


         if(!(nfunc = func_at_addr( LIST_NODE_KEY(cur))))
            error_ret("no func there",-1);

         dump_func_nodes( nfunc , stoplist , depth-1 );

      }
   }

   return(0);
}

int dump_func_edges( func_t * func , 
                     list_t * stoplist , 
                     int depth )
{
   int i;
   list_data_t * data;

   if(!func ||  depth == 0 )
      return 0;

   if(!stoplist)
      error_ret("no stoplist!",-1);

   if(list_retrieve_node( stoplist , OBJ_GET_ADDR(func->obj) ) )
      return(0);
   
   data       = list_new_data();
   data->key  = OBJ_GET_ADDR(func->obj);
   list_insert( stoplist , data );

   if( func->calls )
   {
      list_node_t * cur;
      for( cur = list_root_node(func->calls) ; cur ; cur = cur->next )
      {
         func_t * nfunc;

         if(!(nfunc = func_at_addr( LIST_NODE_KEY(cur) )))
            error_ret("no func there",-1);

         printf("f%s -> f%s\n",func_get_name( func ),
                               func_get_name( nfunc ) );

         /*
         printf("f%s -> f%s\n", addr2dotstr(OBJ_GET_ADDR(func->obj) ),
                                addr2dotstr(OBJ_GET_ADDR(nfunc->obj) ) );
                              */

         dump_func_edges( nfunc , stoplist , depth-1 );

      }
   }

   return(0);
}


int dump_dot_map( addr_t addr , int depth )
{
   func_t * func;
   list_t * stoplist;

   stoplist = new_list(NULL);

   if(!(func = func_at_addr( addr ) ))
      error_ret("no function there",-1);


   printf("digraph test {\n");
   printf("ratio=fill\n");
   /*
   printf("rotate=90\n");
   */
   printf("fontsize=8\n");

   /*
   */
   dump_func_nodes( func , stoplist , depth );
   list_free_list( stoplist , NULL );
   stoplist = new_list(NULL);
   dump_func_edges( func , stoplist , depth  );
   list_free_list(stoplist,NULL);
   printf("}\n");
   return(0);
}

