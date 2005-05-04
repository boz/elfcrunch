#include"elflib.h"
#include"common.h"

/* return a dynamic reloctation table */
void * get_dreloc( elf_t * elf , unsigned  type )
{
   if( ! elf || ( type != DT_RELA && type != DT_REL ) )
      error_ret("bad args",NULL);

   if( type == DT_REL )
      return( get_dt_rel(elf ) );
   else
      error_ret("only doing DT_REL for now",NULL);
}

/* dynamic relocation tables */
Elf32_Rel * get_dt_rel(elf_t * elf) 
{
   Elf32_Dyn * dyn;
   Elf32_Rel * rel;

   unsigned sz,
            ent;

   if( ! elf )
      error_ret( "null args" , NULL );

   if( ! ( dyn = dyn_sym_by_type( elf , DT_REL , 0 ) ) )
      error_ret("no rel table",NULL);

   if( ! ( rel = data_at_addr( elf , dyn->d_un.d_ptr ) ) )
      error_ret("can't get data",NULL);

   if( ! ( dyn = dyn_sym_by_type( elf , DT_RELSZ , 0 )))
      error_ret("can't get sz",NULL);
   sz = dyn->d_un.d_val;

   if( ! ( dyn = dyn_sym_by_type( elf , DT_RELENT , 0 )))
      error_ret("can't get ent",NULL);
   ent = dyn->d_un.d_ptr;

   elf->rel_size    = sz;
   elf->rel_entsize = ent;

   return( ( elf->rel = rel ) );
}

/* return the highest dynamic symbol index
 * we can find in the relocation tables.  
 * Is there another way to obtain the 
 * dynamic symbol table's size?
 */
int rel_max_dsym_indx(  elf_t * elf )
{
   Elf32_Rel * rel;

   int num , 
       maxindx,
       i;

   if( ! elf )
      error_ret("null arg",-1);

   /* XXX: don't forget about rela */
   if(!(rel = get_dreloc( elf , DT_REL ) ) )
      error_ret("can't get rel",-1);
   num = elf->rel_size / elf->rel_entsize;

   maxindx = 0;
   for( i = 0 ; i < num ; i++ , rel++ )
   {
      int tmp;
      tmp = ELF32_R_SYM( rel->r_info );
      if( tmp > maxindx )
         maxindx = tmp;
   }
   return(maxindx);
}

char * reloc_type2str( int type )
{
   static char val[128];
   char * str;
   switch( type ){
     case R_386_NONE     : str = "R_386_NONE     "; break;
     case R_386_32       : str = "R_386_32       "; break;
     case R_386_PC32     : str = "R_386_PC32     "; break;
     case R_386_GOT32    : str = "R_386_GOT32    "; break;
     case R_386_PLT32    : str = "R_386_PLT32    "; break;
     case R_386_COPY     : str = "R_386_COPY     "; break;
     case R_386_GLOB_DAT : str = "R_386_GLOB_DAT "; break;
     case R_386_JMP_SLOT : str = "R_386_JMP_SLOT "; break;
     case R_386_RELATIVE : str = "R_386_RELATIVE "; break;
     case R_386_GOTOFF   : str = "R_386_GOTOFF   "; break;
     case R_386_GOTPC    : str = "R_386_GOTPC    "; break;
     default: str = NULL;
   }

   if( ! str )
   {
      snprintf(val,127,"0x%x",type);
      str = val;
   }
   return(str);
}


