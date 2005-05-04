#include"elflib.h"
#include"common.h"
char * symbol_bind2str( Elf32_Sym * sym )
{
   char * ret;
   if( ! sym )
      error_ret("null arg",NULL);
   switch( ELF32_ST_BIND( sym->st_info ) ) {
     case STB_LOCAL  : ret = "STB_LOCAL "; break;
     case STB_GLOBAL : ret = "STB_GLOBAL"; break;
     case STB_WEAK   : ret = "STB_WEAK  "; break;
     case STB_LOPROC : ret = "STB_LOPROC"; break;
     case STB_HIPROC : ret = "STB_HIPROC"; break;
     default         : ret = "unknown   "; break;
   }
   return( ret );
}

char * symbol_type2str( Elf32_Sym * sym )
{
   char * ret;
   if( ! sym )
      error_ret("null arg",NULL);

   switch( ELF32_ST_TYPE( sym->st_info ) ) {
     case STT_NOTYPE : ret = "STT_NOTYPE "; break;
     case STT_OBJECT : ret = "STT_OBJECT "; break;
     case STT_FUNC   : ret = "STT_FUNC   "; break;
     case STT_SECTION: ret = "STT_SECTION"; break;
     case STT_FILE   : ret = "STT_FILE   "; break;
     case STT_LOPROC : ret = "STT_LOPROC "; break;
     case STT_HIPROC : ret = "STT_HIPROC "; break;
     default         : ret = "unknown    "; break;
   }
   return(ret);
}
