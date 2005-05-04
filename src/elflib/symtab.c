#include"elflib.h"
#include"common.h"
/* symtab via sht */
Elf32_Sym * get_symtab( elf_t * elf )
{
   Elf32_Shdr * shdr;
   char * data;

   if(!elf) error_ret("null args",NULL);
   if(!(shdr = section_by_name( elf , ".symtab" )))
      return ( NULL );
   if(!(data = data_at_offset( elf , shdr->sh_offset )))
      error_ret("can't get data",NULL);
   elf->symtab_len = shdr->sh_size / sizeof( Elf32_Sym ); 
   return( elf->symtab = (Elf32_Sym*)data );
}

Elf32_Sym * sym_by_index( elf_t * elf , int indx )
{
   Elf32_Sym * symtab;
   if( ! elf || indx < 0 || 
       !( symtab = get_symtab( elf ) ) || 
       indx >= elf->symtab_len )
      error_ret("bad args",NULL);
   return( symtab + indx );
}

Elf32_Sym * sym_by_type( elf_t * elf , int type , int indx )
{
   Elf32_Sym * symtab;
   int i;

   if( ! elf || indx < 0 )    
      error_ret("bad args",NULL);

   if(!( symtab = get_symtab( elf ) ) )
      return(NULL);

   for( i = 0 ; i < elf->symtab_len ; i++ )
   {
      int ntype = ELF32_ST_TYPE( symtab[i].st_info );
      if( ( ntype == type) && (indx-- <= 0) )
         return(&symtab[i]);
   }
   return(NULL);
}

Elf32_Sym * sym_by_value( elf_t * elf , int val , int indx )
{
   Elf32_Sym * symtab;
   int i;
   if(!elf || !(symtab = get_symtab(elf)))
      error_ret("bad args",NULL);
   for( i = 0 ; i < elf->symtab_len ; i++ )
   {
      if( symtab[i].st_value == val && (indx--<=0))
         return(&symtab[i]);
   }
   return(NULL);
}

/* return name of symbol at position indx in the symbol table */
char * sym_name_by_index( elf_t * elf , int indx )
{
   Elf32_Sym * sym;
   if( ! elf || ! ( sym = sym_by_index( elf , indx ) ) )
      error_ret("bad args",NULL);
   return( str_by_offset( elf , sym->st_name ) );
}

char * sym_name_by_value( elf_t * elf , int val , int indx )
{
   if(!elf||val<0||indx<0) error_ret ("bad args",NULL);
   Elf32_Sym * sym;
   if( sym = sym_by_value( elf , val , indx ) )
      return( str_by_offset( elf , sym->st_name ) );
   return(NULL);
}


