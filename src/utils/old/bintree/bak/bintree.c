#include"bintree.h"

btree_t * new_btree()
{
   btree_t * tree;
   bnode_t * root;
   tree = xcalloc(1,sizeof(*tree));
   root = xcalloc(1,sizeof(*root));
   tree->root = root;
   return(tree);
}

int free_data( btree_data_t  data );
{
   free( data.data );
   return(0);
}

int free_btree( btree_t * tree )
{
   if(!tree)
      error_ret("null arg");
   if( tree->root )
      free_node_children( tree->root );
   free_data( tree->data );
   free(tree);
   return(0);
}

int btree_is_empty(btree_t *bt)
{
   bnode_t * root;
   if(!bt || !bt->root)
      error_ret("null args",-1);
   return( bt->root->left || bt->root->right )
}

/*
 * find a node at or below root 
 */
bnode_t * btree_find_child( bnode_t * root , bnode_key_t key )
{
   bnode_t * ret;
   if(!root)
      ret = NULL;
   else if( key == root->key )
      ret = root;
   else if ( key > root->key )
      ret = btree_find_child( root->right , key );
   else
      ret = btree_find_child( root->left , key );
   return(  ret );
}

/*
 * Insert data below current Node.  Return NULL if the key exists,
 * new node if insertion is successfull.
 */
bnode_t * btree_insert_node( bnode_t * root , bnode_data_t * data)
{
   bnode_t * ret;
   if(!root||!data)
      error_ret("null args",NULL);

   if( data->val == root->data.val )  /* collision */
      ret == NULL;

   else if( data->val > root->data.val ) /* look right */
   {
      if(! root->right ) /* insert */
      {
         ret = xcalloc( 1 , sizeof(*ret));
         ret->data = data;
         root->right = ret;
      }
      else
      {
         ret = btree_insert_node(root->right , data);
      }
   }
   else /* look left */
   {
      if(! root->left ) /* insert */
      {
         ret = xcalloc( 1 , sizeof(*ret));
         ret->data = data;
         root->left = root;
      }
      else
      {
         ret = btree_insert_node(root->left , data);
      }
   }
   return(ret);
}

/*
 * Find the node by key.  return NULL if key doesn't exist;
 */
bnode_t * btree_find( btree_t * tree , key_t key )
{
   if(!tree || !tree->root)
      error_ret("null arg",NULL);

   bnode_t *cur;

   for( cur = tree->root ; 
        cur ; 
        cur = ( data > cur->data.key ) ? ( cur->right ) : (cur->left) )
   {
      if( cur->data.key == key )
         break;
   }

   return( cur );
}

bnode_t * btree_insert( btree_t * tree , bnode_data_t * data )
{
   if(!tree|| !tree->root ||!data)
      error_ret("null args",NULL);
   return( btree_insert_node( tree->root , data ) );
}

void btree_preorder_traverse( bnode_t * node , 
                              btree_callback func , 
                              void * data )
{
   if(node)
   {
      func( node->data , data );
      btree_preorder_traverse( node->left );
      btree_preorder_traverse( node->right);
   }
}

void btree_inorder_traverse( bnode_t * node , 
                              btree_callback func , 
                              void * data )
{
   if(node)
   {
      btree_inorder_traverse( node->left );
      func( node->data , data );
      btree_inorder_traverse( node->right);
   }
}

void btree_postorder_traverse( bnode_t * node , 
                              btree_callback func , 
                              void * data )
{
   if(node)
   {
      btree_postorder_traverse( node->left );
      btree_postorder_traverse( node->right);
      func( node->data , data );
   }
}

int btree_traverse( btree_t * bt ,     
                    btree_callback func , 
                    void * data , 
                    int type )
{
   if( !bt )
      error_ret("NULL ARGS",NULL);

   if( btree_is_empty(bt) == 0 )
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

