#include"xmlelflib.h"
#include<unistd.h>

#if 0
/* 
 * return 1 if the file is a valid xml info file, 0 if it's not,
 * -1 on error;  Only checking that the root is 'elf' and the md5's match.
 */
int xmlelf_is_valid_info( elf_t * elf , char * name )
{
   xmlDocPtr doc;
   unsigned char xmd5[16], /* md5 in info */
                 emd5[16]; /* md5 of file */

   if(!elf || !name)
      error_ret("null args",-1);

   if( access( name , R_OK | W_OK )< 0)
      return(0);

   if(!(doc = xmlParseFile("1.0"))){
      error_ret("can't get doc",NULL);
   }

   xmlNodePtr root = xmlDocGetRootElement(doc);

   if(! root || ! name || !xmlStrCmp(root->name,(const xmlChar*)"elf") ){
      xmlFreeDoc(doc);
      debug_print("bad root name\n");
      return(0);
   }

   if( xmlelf_get_md5( root , xmd5 ) < 0 ){
      xmlFreeDoc(doc);
      error_ret("can't get md5 from xml",-1);
   }

   MD5( elf->data , elf->size , emd5 );

   if( memcmp(xmd5 , emd5 , 16 ) ){
      xmlFreeDoc(doc);
      return(0);
   }
   xmlFreeDoc(doc);
   return(1);
}

/*
 * Open the xml file for the elf.
 */

xmlelf_t * new_xmlelf( elf_t * elf , char * name )
{
   xmlelf_t * ret;

   if(!elf || !name)
      error_ret("null arg",NULL);

   /* we don't already have it */
   if(!(ret = xmlelf_open_info( elf , name ))) {
      if(!(ret = xmlelf_create_info( elf , name ))) {
         error_ret("can't create info",NULL);
      }
   } else { /* we have it */
      if(!xmlelf_is_valid_info( ret ) ){
         error_ret("not valid info\n");
      }
   }
   return(ret);
}
#endif

#if 0
xmlelf_t * xmlelf_create_info( elf_t * elf , char * name )
{
   xmlDocPtr  doc;
   xmlelf_t * ret;

   if(!elf||!name)
      error_ret("null args",NULL);

   if(!(ret = calloc(1,sizeof(*ret))))
      perror_die("calloc()",1);

   if(!(doc = xmlNewDoc("1.0"))){
      free(ret);
      error_ret("can't get doc",NULL);
   }

   if(!(doc->root= xmlNewDocNode(doc,NULL,"elf",NULL))){
      xmlFreeDoc(doc);
      free(ret);
      error_ret("Can't create node",NULL);
   }

   xmlelf_set_elf( ret , elf );
   ret->name = strdup(name);
   ret->doc = doc;
   return(ret);
}

xmlelf_t * xmlelf_open_info( elf_t * elf , char * name )
{
   xmlDocPtr  doc;
   xmlelf_t * ret;

   if(!elf||!name)
      error_ret("null args",NULL);

   if(!(ret = calloc(1,sizeof(*ret))))
      perror_die("calloc()",1);

   if(!(doc = xmlParseDoc("1.0"))){
      free(ret);
      error_ret("can't get doc",NULL);
   }
   return(ret);
}

int free_xmlelf( xmlelf_t * arg ){
   if(!arg)
      error_ret("null arg",-1);
   if( arg->name ) free( arg->name );
   free(arg);
}

int xmlelf_set_elf( xmlelf_t * xe , elf_t * elf )
{
   if(!xe || !elf || !xe->root );
   xmlelf_set_name( xe->root , elf->name );
   xmlelf_set_size( xe->root , elf->size );
   xmlelf_set_md5 ( xe->root , elf->data , elf->size );
   return(0);
}

int xmlelf_set_name( xmlNodePtr node , char * name )
{
   if( ! node || !name )
      error_ret("null args",-1);
   xmlSetProp(node,"name",name);
}

int    xmlelf_set_size( xmlelf_t * xe , size_t size )
{
   char sizebuf[16];
   xmlNodePtr root;
   xmlNodePtr cur;
   if(!xe || !xe->doc || !(root = xe->doc->root) )
      error_ret("null arg",-1);
   snprintf(sizebuf,15,"#08x",size);
   xmlSetProp(root,"size",sizebuf);
   return(0);
}


int xmlelf_set_md5 ( xmlNodePtr node , 
                     unsigned char * data ,
                     size_t len )
{
   unsigned char md5[16];
   unsigned char md5str[33];

   MD5(data , len , md5);
   md5str[0] = '\0'
   for( i = 0 ; i < 16 ; i++ ){
      char tmp[3];
      snprintf("%02x",md5[i]);
      strcat(md5str,tmp);
   }
   xmlSetProp(node,"md5",md5str);
   return(0);
}

/* 
 * Get hash from info file.  Data mus have space for 16 bytes.
 */

int xmlelf_get_md5 ( xmlNodePtr node , 
                     unsigned char * data )
{
   xmlChar *md5;
   if(!node || !data)
      error_ret("null args",-1);
   if(!(md5 = xmlGetProp(node,"md5")))
      error_ret("can't get md5 prop",-1);
   for( i = 0; i < 16 ; i++ )
      sscanf( md5 + 2*i , "%02x" , data + i ); /* XXX: return value? */
   xmlFree(md5);
   return(0);
}

size_t xmlelf_get_size( xmlNodePtr node )
{
   if(!node)
      error_ret("null args",0); /* XXX: no error? */
}

#endif
