#include "elflib.h"
#include "common.h"
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<string.h>

elf_t * new_elf( char * name )
{
   elf_t * ret;
   char  * data;
   size_t  size;
   ret = malloc( sizeof(*ret) );
   if( ! ret )
      error_ret("malloc()",NULL);
   memset( ret , 0 , sizeof( * ret ) );

   if( name ) {
      if( open_elf(ret,name) < 0 ) {
         free(ret);
         error_ret("open_elf()",NULL);
      }
      if( verify_elf( ret ) < 0 ){
         free_elf(ret);
         error_ret("verify_elf()",NULL);
      }
   }
   return(ret);
}

int free_elf( elf_t * arg )
{
   if( ! arg )
      error_ret("bad arg",-1);
   if( arg->name )
      free( arg->name );
   if( arg->fd && arg->data )
      munmap( arg->data , arg->size );
   free( arg );
}

int open_elf( elf_t * elf , char * name )
{
   int    fd;
   size_t size;
   char   * data;

   if( !( elf && name ) )
      error_ret("null arg",-1);

   if(( fd = open( name , O_RDONLY ) )< 0 )
      error_ret("open()",-1);

   if((size = fgetsize(fd)) < sizeof( Elf32_Ehdr ) ){
      close(fd);
      error_ret("bad file size",-1);
   }

   data = mmap(0 , size , PROT_READ , MAP_PRIVATE , fd , 0 );
   if( ! data ) {
      close(fd);
      error_ret("mmap()",-1);
   }

   elf->name = strdup( name );
   elf->fd   = fd;
   elf->size = size;
   elf->data = data;

   return(size);
}

void * data_at_offset( elf_t * elf , offset_t offset )
{
   if( !elf || !elf->data || offset > elf->size  )
      error_ret("bad args",NULL);
   return( elf->data + offset );
}


void * data_at_addr( elf_t * elf , addr_t vma )
{
   if( ! ( elf && elf->data && elf->pht && elf->ehdr ) )
      error_ret("bad args", NULL);

   int i;
   for( i = 0 ; i < elf->ehdr->e_phnum ; i++ )
   {
      if( elf->pht[i].p_vaddr <= vma &&
          vma < elf->pht[i].p_vaddr + elf->pht[i].p_filesz )
         break;
   }

   if( i < elf->ehdr->e_phnum )
      return( data_at_offset( elf , 
               elf->pht[i].p_offset + vma - elf->pht[i].p_vaddr ) );
   return(NULL);
}

size_t fgetsize(int fd)
{
   struct stat statbuf;
   if( fstat( fd , &statbuf ) < 0 )
      error_ret("fstat failed",0);
   return( statbuf.st_size );
}

