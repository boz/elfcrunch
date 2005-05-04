#include"elflib.h"
#include"common.h"
#define STRING_THRESH 5 /* number of printable chars a string must have */

char * get_dstrtab( elf_t * elf )
{
   Elf32_Dyn * dyn;
   char * ret;
   addr_t addr;
   size_t len;

   if( ! elf )
      error_ret("null args",NULL);

   if( elf->dstrtab )
      return( elf->dstrtab );

   if( ! ( dyn = get_dynamic_symbol( elf , DT_STRTAB , 0 ) ) )
      error_ret( "can't get symtab", NULL );

   addr = dyn->d_un.d_ptr;

   if( ! ( ret = data_at_addr( elf , addr ) ) )
      error_ret( "can't get dstrtab data" , NULL );

   if( ! ( dyn = get_dynamic_symbol( elf , DT_STRSZ , 0 ) ) )
      error_ret( "can't get dstrtab length", NULL );

   elf->dstrtab_len = dyn->d_un.d_val;

   return( ( elf->dstrtab = ret ) );
}

char * dstr_by_offset( elf_t * elf , size_t off )
{
   char * str;
   if( ! elf  )
      error_ret("bad args",NULL);
   if( !( str = get_dstrtab( elf ) ) )
      error_ret("can't get dstrtab",NULL);
   return( strtab_printable( str ) );
}

