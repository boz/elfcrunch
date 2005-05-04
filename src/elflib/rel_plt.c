
#include"elflib.h"
#include"common.h"

void * get_rel_plt( elf_t * elf )
{
   Elf32_Dyn * dyn;
   void * rel_plt;
   size_t  size;
   size_t  entsize;
   unsigned type;

   if( ! elf ) 
      error_ret("bad args",NULL);

   if( elf->rel_plt ) 
      return(elf->rel_plt);

   if( ! ( dyn = dyn_sym_by_type( elf , DT_JMPREL , 0 ) )  )
      error_ret("can't get reloc entry",NULL);

   if( !( rel_plt = data_at_addr( elf , dyn->d_un.d_ptr ) ) )
      error_ret("can't get data",NULL);

   if( ! (dyn = dyn_sym_by_type( elf , DT_PLTRELSZ , 0 ) ) )
      error_ret("can't get size",NULL);

   size = dyn->d_un.d_val;

   if( ! (dyn = dyn_sym_by_type( elf , DT_PLTREL , 0 ) ) )
      error_ret("can't get reloc type",NULL);

   type = dyn->d_un.d_val;

   dyn = 0;
   switch( type ){
     case DT_RELA:
        dyn = dyn_sym_by_type( elf , DT_RELAENT , 0 ); 
        break;
     case DT_REL:
        dyn = dyn_sym_by_type( elf , DT_RELENT , 0 ); 
        break;
     default:
        error_ret("bad relocation type",NULL);
        break;
   }
   if( ! dyn )
      error_ret( " can't get DT_REL[A]NT", NULL );

   entsize = dyn->d_un.d_val;

   elf->rel_plt_type = type;
   elf->rel_plt_size = size;
   elf->rel_plt_entsize = entsize;
   return( ( elf->rel_plt = rel_plt ) );
}

int rel_plt_max_sym_indx( elf_t * elf )
{
   Elf32_Rel * rel_plt;
   if( ! elf || ! ( rel_plt = get_rel_plt( elf ) ) )
      error_ret("bad args",-1);

   int num,
       maxindx,
       i;

   num = elf->rel_plt_size / elf->rel_plt_entsize;
   maxindx = 0;
   for( i = 0 ; i < num ; i++ , rel_plt++ )
   {
      int tmp;
      tmp = ELF32_R_SYM( rel_plt->r_info );
      if( tmp > maxindx )
         maxindx = tmp;
   }
   return(maxindx);
}

