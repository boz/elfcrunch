/* section header string table */

#include"elflib.h"
#include"common.h"
#define STRING_THRESH 5 /* number of printable chars a string must have */

char * get_shstrtab( elf_t * elf )
{
   Elf32_Shdr * shdr;
   Elf32_Ehdr * ehdr;

   char * ret;
   addr_t addr;
   size_t len;

   if( ! elf )
      error_ret("null args",NULL);

   if( elf->shstrtab )
      return( elf->shstrtab );

   if(!( ehdr  = get_ehdr( elf )))
      error_ret("Can't get elf",NULL);

   if(! has_sht(elf) )
      return( NULL );


   if( ! ( shdr = get_section_at_index( elf , ehdr->e_shstrndx ) ))
      error_ret( "can't get ssymtab", NULL );

   if( ! ( ret = data_at_offset( elf , shdr->sh_offset ) ) )
      error_ret( "can't get strtab data" , NULL );
   elf->shstrtab_len = shdr->sh_size;

   return( ( elf->shstrtab = ret ) );
}

char * shstr_by_offset( elf_t * elf , size_t off )
{
   char * str;
   if( ! elf  )
      error_ret("bad args",NULL);
   if( !( shstrtab = get_shstrtab( elf ) ) )
      error_ret("can't get strtab",NULL);
   if( off > elf->shstrtab_len )
      error_ret("overflow",NULL);
   return( strtab_printable( shstrtab + off ) );
}

