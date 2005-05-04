
#include<unistd.h>
#include<string.h>
#include<openssl/md2.h>
#include<libxml/xmlstring.h>
#include"xmlelflib.h"
#include"elflib.h"
#include"common.h"

/*
 * Set a node's 'size' attribute
 */
int    xmlelf_set_size( xmlelf_t * xe , size_t size )
{
   char sizebuf[16];
   xmlNodePtr root;
   xmlNodePtr cur;
   if(!xe || !xe->doc || !(root = xe->root) )
      error_ret("null arg",-1);
   snprintf(sizebuf,15,"0x%08x",size);
   xmlSetProp(root,"size",sizebuf);
   return(0);
}


/*
 * Set a node's 'name' attribute.  I don't think this is necessary.
 */
int xmlelf_set_name( xmlNodePtr node , char * name )
{
   if( ! node || !name )
      error_ret("null args",-1);
   xmlSetProp(node,"name",name);
}


/*
 * Set a node's 'md5' attribute to the hash of data (len bytes)
 */
int xmlelf_set_md5 ( xmlNodePtr node , 
                     unsigned char * data ,
                     size_t len )
{
   unsigned char md5[16];
   unsigned char md5str[33];

   MD5(data , len , md5);

   md5str[0] = '\0';

   int i;
   for( i = 0 ; i < 16 ; i++ ){
      char tmp[4];
      snprintf(tmp,3,"%02x",md5[i]);
      strcat(md5str,tmp);
   }
   xmlSetProp(node,"md5",md5str);
   return(0);
}

int xmlelf_set_elf( xmlelf_t * xe , elf_t * elf )
{
   if(!xe || !elf || !xe->root );
   xmlelf_set_name( xe->root , elf->name );
   xmlelf_set_size( xe  , elf->size );
   xmlelf_set_md5 ( xe->root , elf->data , elf->size );
   return(0);
}

/* 
 * return 1 if the file is a valid xml info file, 0 if it's not
 */

int xmlelf_is_valid_info(  xmlelf_t * xe )
{
   unsigned char xmd5[16], /* md5 in info */
                 emd5[16]; /* md5 of file */

   if( !xe || ! xe->root || !xe->elf || !xe->root->name || !xe->elf )
      error_ret("null args",-1);

   if(xmlStrcmp(xe->root->name,(const xmlChar*)"elf") )
      error_ret("root node not \'elf\'",0);

   /* check the md5 */
   if( xmlelf_get_md5( xe->root , xmd5 ) < 0 )
      error_ret("can't get md5 from xml",0);
   MD5( xe->elf->data , xe->elf->size , emd5 );
   if( memcmp(xmd5 , emd5 , 16 ) )
      error_ret("md5's don't match",0);

   /* check the name? */
   char * name;
   if(!(name = xmlelf_get_name( xe->root )))
      error_ret("no name attribute",-1);
   if( strcmp( name , xe->elf->name ) )
      error_ret("names don't match",-1);

   return(1);
}

/*
 * Open an existing info file.  
 */
xmlelf_t * xmlelf_open_info( elf_t * elf , char * name )
{
   xmlDocPtr  doc;
   xmlelf_t * ret;

   if(!elf||!name)
      error_ret("null args",NULL);

   if(!(ret = calloc(1,sizeof(*ret))))
      perror_die("calloc()",1);

   if(!(doc = xmlParseFile(name))){
      free(ret);
      error_ret("can't get doc",NULL);
   }
   ret->elf  = elf;
   ret->doc  = doc;
   ret->root = doc->children;
   ret->name = strdup(name);
   return(ret);
}

/*
 * Create a new xmlelf info file.
 */

xmlelf_t * xmlelf_create_info( elf_t * elf , char * name )
{
   xmlDocPtr  doc;
   xmlelf_t * ret;

   if(!elf||!name)
      error_ret("null args",NULL);

   if(!(ret = calloc(1,sizeof(*ret))))
      perror_die("calloc()",1);

   if(!(doc  = xmlNewDoc("1.0"))){
      free(ret);
      error_ret("can't get doc",NULL);
   }

   if(!(ret->root = xmlNewDocNode( doc ,NULL,"elf",NULL))){
      xmlFreeDoc(doc);
      free(ret);
      error_ret("Can't root node",NULL);
   }

   doc->children = ret->root;
   ret->name = strdup(name);
   ret->doc = doc;
   xmlelf_set_elf( ret , elf );
   return(ret);
}

/*
 * Open the xml file for the elf.
 */

xmlelf_t * new_xmlelf( elf_t * elf , char * name )
{
   xmlelf_t * ret;
   if(!elf || !name)
      error_ret("null arg",NULL);

   if( access( name , R_OK | W_OK  ) == 0 ) 
   {
      if(!(ret = xmlelf_open_info( elf , name )))
         error_ret("can't open info",NULL);
      if(!xmlelf_is_valid_info( ret ) )
      {
         xmlFreeDoc(ret->doc);
         free(ret);
         error_ret("not valid info file",NULL);
      }
   }  
   else 
   {
      /* create a new one */
      if(!(ret = xmlelf_create_info( elf , name )))
         error_ret("can't create info",NULL);
   }
   return(ret);
}

int free_xmlelf( xmlelf_t * arg ){
   if(!arg)
      error_ret("null arg",-1);
   if( arg->name ) free( arg->name );
   free(arg);
}


/* 
 * Get hash from info file.  Data mus have space for 16 bytes.
 */

int xmlelf_get_md5 ( xmlNodePtr node , unsigned char * data )
{
   xmlChar *md5;
   int i;

   if(!node || !data)
      error_ret("null args",-1);
   if(!(md5 = xmlGetProp(node,"md5")))
      error_ret("can't get md5 prop",-1);

   for( i = 0; i < 16 ; i++ )
      sscanf( md5 + 2*i , "%02x" , data + i ); /* XXX: return value? */
   xmlFree(md5);

   return(0);
}

char * xmlelf_get_name ( xmlNodePtr node )
{
   static char  ret[MAX_NAME];
   char * name;

   if(!node )
      error_ret("null args",NULL);
   if(!(name = xmlGetProp(node,"name")))
      error_ret("can't get md5 prop",NULL);
   strncpy( ret , name , sizeof(ret) - 1 );
   return( name ? ret : NULL );
}

/*
 * Get size from info file. 
 * return 0 on error?
 */
size_t xmlelf_get_size( xmlNodePtr node )
{
   if(!node)
      error_ret("null args",0); /* XXX: no error? */
   return(0);
}

int xmlelf_write_info( xmlelf_t * xe )
{
   if(!xe || !xe->doc || !xe->name )
      error_ret("null args",-1);
   xmlSaveFormatFile(xe->name,xe->doc,0);
   xmlFreeDoc(xe->doc);
   return(0);
}

