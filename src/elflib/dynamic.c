#include "elflib.h"
#include"common.h"

Elf32_Dyn * get_dynamic( elf_t * elf )
{
   Elf32_Dyn   * dynamic;
   Elf32_Phdr  * phdr;

   if( !elf || !elf->data || ( !elf->pht && ! get_pht( elf ) ) )   
      error_ret("bad args",0);

   if( elf->dynamic )
      return(elf->dynamic);

   phdr = phdr_by_type( elf , PT_DYNAMIC , 0 );
   if( ! phdr )
      error_ret("can't get phdr",NULL);

   dynamic = data_at_offset( elf , phdr->p_offset );

   if( ! dynamic )
      error_ret("bad addr",0);

   return((elf->dynamic = dynamic));
}

Elf32_Dyn * dyn_sym_by_type( elf_t * elf , int type , int indx )
{
   if( ! elf || indx < 0 )
      error_ret("bad args",NULL);
   Elf32_Dyn * dyn;
   if( ! (dyn = get_dynamic( elf)) )  
      error_ret("can't get dynamic section",NULL);
   for( ; dyn->d_tag != DT_NULL ; dyn++ )
      if( dyn->d_tag == type  && indx-- >= 0 )
         return(dyn);
   return(NULL);
}

char * dyn_tag2str( int tag )
{
   char * str;
   switch( tag ) {
      case  DT_NULL    : str = "DT_NULL    "; break;
      case  DT_NEEDED  : str = "DT_NEEDED  "; break;
      case  DT_PLTRELSZ: str = "DT_PLTRELSZ"; break;
      case  DT_PLTGOT  : str = "DT_PLTGOT  "; break;
      case  DT_HASH    : str = "DT_HASH    "; break;
      case  DT_STRTAB  : str = "DT_STRTAB  "; break;
      case  DT_SYMTAB  : str = "DT_SYMTAB  "; break;
      case  DT_RELA    : str = "DT_RELA    "; break;
      case  DT_RELASZ  : str = "DT_RELASZ  "; break;
      case  DT_RELAENT : str = "DT_RELAENT "; break;
      case  DT_STRSZ   : str = "DT_STRSZ   "; break;
      case  DT_SYMENT  : str = "DT_SYMENT  "; break;
      case  DT_INIT    : str = "DT_INIT    "; break;
      case  DT_FINI    : str = "DT_FINI    "; break;
      case  DT_SONAME  : str = "DT_SONAME  "; break;
      case  DT_RPATH   : str = "DT_RPATH   "; break;
      case  DT_SYMBOLIC: str = "DT_SYMBOLIC"; break;
      case  DT_REL     : str = "DT_REL     "; break;
      case  DT_RELSZ   : str = "DT_RELSZ   "; break;
      case  DT_RELENT  : str = "DT_RELENT  "; break;
      case  DT_PLTREL  : str = "DT_PLTREL  "; break;
      case  DT_DEBUG   : str = "DT_DEBUG   "; break;
      case  DT_TEXTREL : str = "DT_TEXTREL "; break;
      case  DT_JMPREL  : str = "DT_JMPREL  "; break;
      case  DT_LOPROC  : str = "DT_LOPROC  "; break;
      case  DT_HIPROC  : str = "DT_HIPROC  "; break;
      default: str = "unknown" ; break;
   };
   return( str );
}

