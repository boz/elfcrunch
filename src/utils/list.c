#include"list.h"
#include"common.h"

/* return >0 if a > b
 *        0  if a == b
 *        <0 if a < b 
 */

int list_default_keycheck( list_data_t * a , list_data_t * b )
{
   return( a->key - b->key );
}

list_t * new_list( list_cmp_func func )
{
   list_t * list;
   list = xcalloc( 1,sizeof(*list));
   list->cmp = (func) ? (func) : (list_default_keycheck);
   return(list);
}

int list_free_list( list_t * list , free_cb cb )
{
   if(!list) error_ret("bad args",-1);

   list_node_t * cur,
               * prev;

   prev = NULL;
   cur = (&list->head)->next;
   for( cur = (&list->head)->next ; cur ; cur = cur->next ){
      if(cb) (*cb)(cur->data.data);
      if(prev)
      {
         free( prev );
      }
      prev = cur;
   }
   if(prev)
   {
      free( prev );
   }
   free( list );
   return(0);
}

int list_insert( list_t * list , list_data_t * data )
{
   nkey_t key;
   list_node_t * node;
   list_node_t * cur,
               * prev;

   if(!data||!list) error_ret("null args",-1);

   prev = &list->head;
   for( cur = prev->next ; cur; cur = cur->next )
   {
      if( list->cmp( data , &cur->data ) <= 0 )
         break;
      prev = cur;
   }

   node = list_new_node();
   prev->next = node;
   node->next = cur;
   node->data = *data;
   list->size++;
   return(0);
}

list_node_t * list_retrieve_node( list_t * list , nkey_t key )
{
   list_node_t * cur;

   if(!list) error_ret("null args",NULL);
   for( cur = (&list->head)->next ; cur ; cur = cur->next ){
      if( LIST_NODE_KEY(cur) == key )
         return( cur );
   }
   return(NULL);
}

void * list_retrieve_data( list_t * list , nkey_t key )
{
   list_node_t * node;
   if((node = list_retrieve_node( list , key )))
      return( node->data.data );
   return(NULL);
}

list_node_t * list_node_by_index( list_t * list , int indx )
{
   int curindx = 0;
   list_node_t * cur;
   if(!list || indx >= list->size )
      error_ret("bad args",NULL);
   for( cur = (&list->head)->next ; cur ; cur = cur->next ){
      if( curindx++ == indx )
         return( cur );
   }
   return(NULL);
}

list_data_t * list_data_by_index( list_t * list , int indx )
{
   list_node_t * node;
   if(!list||indx>=list->size)
      error_ret("bad args",NULL);
   if((node = list_node_by_index(list,indx)))
      return( node->data.data );
   return(NULL);
}
int list_map_over_data( list_t * list , list_callback cb , void * arg )
{
   list_node_t * cur;
   if(!cb) error_ret("bad args",-1);
   if( list ) {
      for( cur = (&list->head)->next ; cur ; cur = cur->next ){
         if( (*cb)(cur->data.data,arg) <0)
            return(-1);
      }
   }
   return(0);
}


