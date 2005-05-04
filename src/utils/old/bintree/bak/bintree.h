#ifndef BINTREE_H
#define BINTREE_H

typedef struct 
{
   uint32_t key;
   void   * data;
} btree_data_t;

typedef struct bnode_t_
{
   btree_data data;
   bnode_t_ * lchld;
   bnode_t_ * rchld;
} bnode_t;

typedef int (*bnode_callback) ( bnode_t * node , void * data );
/* callback for traversals */

typedef struct btree_t_
{
   bnode_t * root;
} btree_t;

btree_t * new_btree();

int free_data( btree_data_t  data );
int free_btree( btree_t * tree );
int btree_is_empty(btree_t *bt);

int btree_traverse( btree_t * bt , btree_callback func , int type );
/* 
 * type:
 * 0: preorder
 * 1: inorder
 * 2: postorder
 */

bnode_t * btree_find_child( bnode_t * root , bnode_key_t key );
bnode_t * btree_insert_node( bnode_t * root , bnode_data_t * data);
bnode_t * btree_find( btree_t * tree , key_t key );
bnode_t * btree_insert( btree_t * tree , bnode_data_t * data );

bnode_t * btree_attach_left( bnode_t * root , bnode_t * left );
bnode_t * btree_attach_right( bnode_t * root , bnode_t * right );

#endif  /* BINTREE_H */
