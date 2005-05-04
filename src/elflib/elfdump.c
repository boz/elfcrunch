#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"elflib.h"
#include"dumps.h"

void usage( char * name )
{
   printf("Usage: %s <option ...> file\n",name);
   printf("Options:\n");
   exit(1);
}

int main( int argc , char ** argv )
{
   if( argc != 2 )
      usage(argv[0]);

   char * file_name = argv[1];

   elf_t * elf;
   elf = new_elf( file_name );
   if(  ! elf ) {
      fprintf(stderr,"can't load %s\n",argv[1]);
      exit(1);
   }



   dump_dynamic ( elf ); /* _DYNAMIC entrys      */


#if 0
   dump_symtab  ( elf ); /* symbol table */
   dump_dsymtab ( elf ); /* dynamic symbol table */
   dump_rel_plt ( elf ); /* plt relocations      */
   dump_ehdr    ( elf ); 
   dump_pht     ( elf );
   dump_sht     ( elf );

   dump_shstrtab( elf ); /* section string table */
   dump_dstrtab ( elf ); /* dynamic string table */
#endif

   exit(0);
}

