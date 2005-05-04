#ifndef XMLELFLIB_H
#define XMLELFLIB_H
#include<libxml/xpointer.h>
#include"elflib.h"

typedef struct
{
   char      *name; /* name of file */
   xmlDocPtr  doc;  /* document     */
   xmlNodePtr root; /* root */
   elf_t  * elf;    /* elf file */
} xmlelf_t;

xmlelf_t * new_xmlelf( elf_t * elf , char * name );
int        free_xmlelf( xmlelf_t * arg );

xmlelf_t * xmlelf_open_info  ( elf_t * elf , char * name );
xmlelf_t * xmlelf_create_info( elf_t * elf , char * name );

int xmlelf_write_info( xmlelf_t * xe );

int xmlelf_is_valid_info(  xmlelf_t * xe );
int xmlelf_set_elf( xmlelf_t * xe , elf_t * elf );

/* break these off? */
int    xmlelf_set_size( xmlelf_t * xe , size_t size );
char * xmlelf_get_name( xmlNodePtr node );
int    xmlelf_set_name( xmlNodePtr node , char * name );
size_t xmlelf_get_size( xmlNodePtr node );
int    xmlelf_set_md5 ( xmlNodePtr node , unsigned char * data , size_t len );
int    xmlelf_get_md5 ( xmlNodePtr node , unsigned char * data );


typedef struct
{
   char * path;
   int (*get)( xmlNodePtr node );
   int (*set)( xmlNodePtr node );
} inforeg_t ; 

#endif /* XMLELFLIB_H */

