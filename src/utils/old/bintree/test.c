#include"bintree.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

int print_node( btree_data_t * data , void * arg )
{
   printf("%02d\t%s\n",data->key, (char*)data->data);
   return(0);
}

int main( int argc , char *argv[] )
{
   btree_data_t  data;
   btree_t * tree;
   tree = new_btree();

   data.key  = 10;
   data.data = strdup( "ten" );
   btree_insert(tree,&data);

   data.key  = 15;
   data.data = strdup( "fifteen" );
   btree_insert(tree,&data);

   data.key  = 12;
   data.data = strdup( "twelve" );
   btree_insert(tree,&data);

   data.key  = 20;
   data.data = strdup( "twenty" );
   btree_insert(tree,&data);

   data.key  = 5;
   data.data = strdup( "five" );
   btree_insert(tree,&data);

   data.key  = 8;
   data.data = strdup( "eight" );
   btree_insert(tree,&data);

   data.key  = 3;
   data.data = strdup( "three" );
   btree_insert(tree,&data);

   printf("preorder:\n");
   btree_traverse(tree,print_node,NULL,0);
   printf("inorder:\n");
   btree_traverse(tree,print_node,NULL,1);
   printf("postorder:\n");
   btree_traverse(tree,print_node,NULL,2);

   free_btree(tree);
   return(0);
}
