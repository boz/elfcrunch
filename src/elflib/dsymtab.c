#include<strings.h>
#include"elflib.h"
#include"common.h"

/* Symbol table from the _DYNAMIC segment */
Elf32_Sym * get_dsymtab( elf_t * elf )
{
   Elf32_Dyn * dyn;
   Elf32_Sym * dsymtab;
   addr_t   addr;
   size_t   size;

   if( ! elf  )
      error_ret("null args",NULL);

   if( elf->dsymtab ) 
      return( elf->dsymtab);

   if( ! (dyn = dyn_sym_by_type( elf , DT_SYMTAB , 0 ) ))
      error_ret("can't get dsymtab entry",NULL);

   if( ! (dsymtab = data_at_addr( elf , dyn->d_un.d_ptr ) ) )
      error_ret("can't get dsymtab",NULL);

   elf->dsymtab_len = get_dsymcount( elf );

   return(( elf->dsymtab = dsymtab ));
}

Elf32_Sym * dsym_by_index( elf_t * elf , int indx )
{
   Elf32_Sym * dsymtab;
   if( ! elf || indx < 0 || 
       !( dsymtab = get_dsymtab( elf ) ) || 
       indx >= elf->dsymtab_len )
      error_ret("bad args",NULL);
   return( dsymtab + indx );
}

/* return name of symbol at position indx in the symbol table */
char * dsym_name_by_index( elf_t * elf , int indx )
{
   Elf32_Sym * sym;
   if( ! elf || ! ( sym = dsym_by_index( elf , indx ) ) )
      error_ret("bad args",NULL);
   return( dstr_by_offset( elf , sym->st_name ) );
}

int get_dsymcount( elf_t * elf )
{
   if( ! elf )
      error_ret("null arg",-1);

   int relsc,
       relpltsc,
       max;

   /* XXX this is lame */
   if((relpltsc = rel_plt_max_sym_indx( elf )) < 0 )
      error_ret("rel_plt symcount", -1 );

   if((relsc = rel_max_dsym_indx( elf ))< 0)
      error_ret("rel symcount",-1);
   max = ( relpltsc > relsc ) ? relpltsc : relsc;
   return(max);
}

Elf32_Sym * dsym_by_type( elf_t * elf , int type , int indx )
{
   Elf32_Sym * symtab;
   int i;

   if( ! elf || indx < 0 || !( symtab = get_dsymtab( elf ) ) )
      error_ret("bad args",NULL);

   for( i = 0 ; i < elf->dsymtab_len ; i++ )
   {
      int ntype = ELF32_ST_TYPE( symtab[i].st_info );
      if( ( ntype == type) && (indx-- <= 0) )
         return(&symtab[i]);
   }
   return(NULL);
}
