#ifndef LIST_H
#define LIST_H
#include"data_structs.h"
#include"libutil.h"

typedef struct list_data_
{
   nkey_t key;
   void * data;
} list_data_t;
#define list_new_data() xcalloc( 1 , sizeof( list_data_t ))

typedef struct list_node_
{
   struct list_node_ * next;
   list_data_t data;
} list_node_t;
#define list_new_node() xcalloc( 1 , sizeof( list_node_t ))

typedef int (*list_cmp_func) (list_data_t *a,list_data_t *b) ;
typedef int (*list_callback) (void * a,void * arg);
typedef int (*free_cb)(void * a);

#define list_root_node(l) (l)->head.next


typedef struct list_
{
   list_node_t head;
   int size;
   list_cmp_func cmp;
} list_t;

list_t * new_list( list_cmp_func func );
int list_free_list( list_t * list , free_cb cb );

int list_default_keycheck( list_data_t * a , list_data_t * b );

int list_insert( list_t * list , list_data_t * data );

list_node_t * list_retrieve_node( list_t * list , nkey_t key );
void        * list_retrieve_data( list_t * list , nkey_t key );


list_node_t * list_node_by_index( list_t * list , int indx );
list_data_t * list_data_by_index( list_t * list , int indx );

int list_map_over_data( list_t * list , list_callback , void * arg );

#define LIST_DATA_KEY(data) (data).key
#define LIST_NODE_KEY(node) LIST_DATA_KEY( node->data )

#endif /* LIST_H */
