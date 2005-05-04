#include"bintree.h"
#include"libutil.h"

btree_t * new_btree()
{
   btree_t * tree;
   tree = xcalloc(1,sizeof(*tree));
   tree->free_node_data = free_data;
   return(tree);
}

void free_data( bnode_t * node )
{
   if(node)
      xfree( node->data.data );
}

void btree_destroy( btree_t * tree , bnode_t * root )
{
   if( root )
   {
      btree_destroy( tree , root->lchld  );
      btree_destroy( tree , root->rchld );
      tree->free_node_data( root );
      xfree(root);
   }
}

int free_btree( btree_t * tree )
{
   if(!tree)
      error_ret("null arg",-1);
   if( tree->root )
      btree_destroy( tree , tree->root );
   xfree(tree);
   return(0);
}

int btree_is_empty(btree_t *bt)
{
   if(!bt)
      error_ret("null args",-1);
   return( (bt->root) ? ( 0 ) : ( 1 ) );
}

/*
 * find a node at or below root 
 */
bnode_t * btree_find_child( bnode_t * root , nkey_t key )
{
   bnode_t * ret;
   if(!root)
      ret = NULL;
   else if( key == root->data.key )
      ret = root;
   else if ( key > root->data.key )
      ret = btree_find_child( root->rchld , key );
   else
      ret = btree_find_child( root->lchld , key );
   return(  ret );
}

/*
 * Insert data below current Node.  Return NULL if the key exists,
 * new node if insertion is successfull.
 */
bnode_t * btree_insert_node( bnode_t * root , btree_data_t  * data)
{
   bnode_t * ret;
   if(!root||!data)
      error_ret("null args",NULL);

   if( data->key == root->data.key )  /* collision */
      ret = NULL;

   else if( data->key > root->data.key ) /* look right */
   {
      if(! root->rchld ) /* insert */
      {
         ret = xcalloc( 1 , sizeof(*ret));
         ret->data = *data;
         root->rchld = ret;
      }
      else
      {
         ret = btree_insert_node(root->rchld , data);
      }
   }
   else /* look left */
   {
      if(! root->lchld ) /* insert */
      {
         ret = xcalloc( 1 , sizeof(*ret));
         ret->data = *data;
         root->lchld = ret;
      }
      else
      {
         ret = btree_insert_node(root->lchld , data);
      }
   }
   return(ret);
}

/*
 * Find the node by key.  return NULL if key doesn't exist;
 */
bnode_t * btree_find( btree_t * tree , nkey_t key )
{
   if(!tree || !tree->root)
      error_ret("null arg",NULL);

   bnode_t *cur;

   for( cur = tree->root ; 
        cur ; 
        cur = ( key > cur->data.key ) ? ( cur->rchld ) : (cur->lchld) )
   {
      if( cur->data.key == key )
         break;
   }

   return( cur );
}

bnode_t * btree_insert( btree_t * tree , btree_data_t * data )
{
   bnode_t * ret;
   if(!tree|| !data)
      error_ret("null args",NULL);
   if(!tree->root){
      tree->root = xcalloc( 1 , sizeof(bnode_t));
      tree->root->data = *data;
      ret = tree->root;
   }
   else
      ret = btree_insert_node( tree->root , data ) ;

   return(ret);
}

void btree_preorder_traverse( bnode_t * node , 
                              btree_callback func , 
                              void * data )
{
   if(node)
   {
      func( &(node->data) , data );
      btree_preorder_traverse( node->lchld , func , data);
      btree_preorder_traverse( node->rchld , func , data);
   }
}

void btree_inorder_traverse( bnode_t * node , 
                              btree_callback func , 
                              void * data )
{
   if(node)
   {
      btree_inorder_traverse( node->lchld , func , data);
      func( &(node->data) , data );
      btree_inorder_traverse( node->rchld , func , data);
   }
}

void btree_postorder_traverse( bnode_t * node , 
                              btree_callback func , 
                              void * data )
{
   if(node)
   {
      btree_postorder_traverse( node->rchld , func , data);
      btree_postorder_traverse( node->lchld , func , data);
      func( &(node->data) , data );
   }
}

int btree_traverse( btree_t * bt ,     
                    btree_callback func , 
                    void * data , 
                    int type )
{
   if( !bt )
      error_ret("NULL ARGS",-1);

   if( btree_is_empty(bt)>0 )
      error_ret("empty tree",0); /* not necessarily an error */

   switch( type ) {
     case 0:
        btree_preorder_traverse( bt->root , func , data );
        break;
     case 1:
        btree_inorder_traverse( bt->root , func , data );
        break;
     case 2:
        btree_postorder_traverse( bt->root , func , data );
        break;
     default:
        error_ret("bad traversal type",-1);
   }
   return( 0 );
}

