#include"elflib.h"
#include"common.h"

char * strtab_printable( char * str )
{
   /* came accross an encrypted strtab */
   int i;
   for( i = 0 ; i < STRING_THRESH ; i++ ) {
      if( str[i] == '\0' )
         break;
      else if( ! isgraph( str[i] ) )
         return("not printable");
   }
   return(str);
}

/* section header string table */
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

   if( ! ( shdr = section_by_index( elf , ehdr->e_shstrndx ) ))
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
   if( !( str = get_shstrtab( elf ) ) )
      error_ret("can't get strtab",NULL);
   if( off > elf->shstrtab_len )
      error_ret("overflow",NULL);
   return( strtab_printable( str + off ) );
}

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

   if( ! ( dyn = dyn_sym_by_type( elf , DT_STRTAB , 0 ) ) )
      error_ret( "can't get symtab", NULL );

   addr = dyn->d_un.d_ptr;

   if( ! ( ret = data_at_addr( elf , addr ) ) )
      error_ret( "can't get dstrtab data" , NULL );

   if( ! ( dyn = dyn_sym_by_type( elf , DT_STRSZ , 0 ) ) )
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

   if( off >= elf->dstrtab_len )
      error_ret("overflow","");
   return( strtab_printable( str + off ) );
}

char * get_strtab( elf_t * elf )
{
   Elf32_Shdr * shdr;
   Elf32_Ehdr * ehdr;

   char * ret;
   addr_t addr;
   size_t len;

   if( ! elf )
      error_ret("null args",NULL);

   if( elf->strtab )
      return( elf->strtab );

   if(!( ehdr  = get_ehdr( elf )))
      error_ret("Can't get elf",NULL);

   if(! has_sht(elf) )
      return( NULL );

   if( ! ( shdr = section_by_name( elf , ".strtab" ) ))
      error_ret( "can't get strtab", NULL );

   if( ! ( ret = data_at_offset( elf , shdr->sh_offset ) ) )
      error_ret( "can't get strtab data" , NULL );
   elf->strtab_len = shdr->sh_size;

   return( ( elf->strtab = ret ) );
}

char *  str_by_offset( elf_t * elf , size_t off )
{
   char * str;
   if( ! elf  )
      error_ret("bad args","");
   if( !( str = get_strtab( elf ) ) )
      error_ret("can't get dstrtab","");
   if( off >= elf->strtab_len )
      error_ret("overflow","");
   return( strtab_printable( str + off ) );
}
