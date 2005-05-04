#ifndef BINTREE_H
#define BINTREE_H
#include<stdint.h>
#include"data_structs.h"

typedef struct 
{
   nkey_t    key;
   void   * data;
} btree_data_t ;

typedef struct bnode_t_
{
   btree_data_t  data;
   struct bnode_t_ * lchld;
   struct bnode_t_ * rchld;
} bnode_t;

typedef int (*btree_callback) ( btree_data_t *  , void *  );
/* callback for traversals */

typedef struct btree_t_
{
   bnode_t * root;
   void    (*free_node_data)( bnode_t * node );
} btree_t;

btree_t * new_btree();

void free_data( bnode_t * );
/* Default method to delete data.  free( node->data ); */

int  free_btree( btree_t * tree );
/* free entire tree */

int  btree_is_empty(btree_t *bt);

int btree_traverse( btree_t * bt , btree_callback func , void *  , int type );
/* 
 * type:
 * 0: preorder
 * 1: inorder
 * 2: postorder
 */

bnode_t * btree_find  ( btree_t * tree , nkey_t key );
/* find a node given a key */

void * btree_find_data( btree_t * tree , nkey_t key );

int btree_delete( btree_t * tree , nkey_t key );

/* delete the node containing key */

bnode_t * btree_insert( btree_t * tree , btree_data_t * data );
/* Insert data.  Return NULL if key exists */

bnode_t * btree_find_child( bnode_t * root , nkey_t key );
/* find a child below root.  return NULL if key is not found */

bnode_t * btree_insert_node( bnode_t * root , btree_data_t * data);
/* insert a node below root */

bnode_t * btree_attach_left( bnode_t * root , bnode_t * left );
bnode_t * btree_attach_right( bnode_t * root , bnode_t * right );

#endif  /* BINTREE_H */
