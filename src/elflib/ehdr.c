#include"elflib.h"
#include"common.h"

Elf32_Ehdr * get_ehdr( elf_t * elf )
{
   if( ! elf || !elf->data )
      error_ret("bad arg",NULL);

   if( elf->ehdr ){
      return(elf->ehdr);
   }
   return( elf->ehdr = elf->data );
}

          
int verify_elf( elf_t * elf )
{
   /* bother getting phdr & shdr ? */
   Elf32_Ehdr * ehdr;
   Elf32_Phdr * phdr;
   Elf32_Shdr * shdr;

   if( ! ( elf && elf->data && elf->size > sizeof( Elf32_Ehdr ) ) )
      error_ret("bad elf",-1);

   ehdr = get_ehdr( elf );

   if( memcmp( ehdr , ELFMAG , SELFMAG ) )
      error_ret("bad magic",-1);

   if( ! ( get_pht(elf) && get_sht(elf) ) )
      error_ret("can't get phdr,shdr",-1);

   return(0);
}
