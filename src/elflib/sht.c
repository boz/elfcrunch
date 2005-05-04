#include"elflib.h"
#include"common.h"

int has_sht( elf_t * elf )
{
   Elf32_Ehdr * ehdr;
   if(!elf) error_ret("null arg",0);
   if(!(ehdr = get_ehdr( elf ) ))
      error_ret("can't get ehdr",0);
   return( ehdr->e_shnum != 0  );
}

Elf32_Shdr * get_sht( elf_t * elf )
{
   offset_t off;
   Elf32_Shdr * ret;

   if( ! elf )
      error_ret("bad arg",NULL);
   if( elf->sht )
      return(elf->sht);

   if(!has_sht(elf)) {
      elf->sht_count = 0;
      return( elf->sht = NULL );
   }

   if( ! elf->ehdr && ! get_ehdr( elf ) )
      error_ret("can't get ehdr",NULL);

   off = elf->ehdr->e_shoff;
   ret = data_at_offset( elf , off );
   if( ! ret )
      error_ret("can't get data",NULL);
   elf->sht_count = elf->ehdr->e_shnum;
   return( elf->sht = ret );
}

Elf32_Shdr * section_by_index( elf_t * elf , int indx )
{
   Elf32_Shdr * shdr;

   if( !elf || indx < 0 )
      error_ret("bad args",NULL);
   if(!(shdr = get_sht( elf ) ))
      error_ret("can't get ehdr",NULL);

   return( ( indx < elf->sht_count ) ? (shdr+indx) : (NULL) );
}

Elf32_Shdr * section_by_flag( elf_t * elf , int flag , int indx )
{
   Elf32_Shdr * shdr;
   int i;
   if(!elf||indx<0) error_ret("bad args",NULL);
   if(!(shdr = get_sht(elf)))
      return(NULL);

   for(i=0;i<elf->sht_count;i++)
   {
      if( (shdr->sh_flags & flag) && (indx-- <= 0)) 
         return(shdr);
      shdr++;
   }
   return(NULL);
}


Elf32_Shdr * section_by_name( elf_t * elf , char * name )
{
   Elf32_Shdr * sht;
   char * cur;
   int size,i;
   if(!elf||!name) error_ret("null args",NULL);

   if(!(sht = get_sht( elf ) )) 
      error_ret("Can't get sht",NULL);

   size = elf->sht_count;

   for( i = 0 ; i < size ; i++ ){
      if(!(cur = shstr_by_offset( elf , sht[i].sh_name)))
         continue; /* shouldn't happen */
      if(!strcmp( cur , name ))
         return( &sht[i] );
   }
   return(NULL);
}


Elf32_Shdr * section_by_type( elf_t * elf , Elf32_Word type , int indx )
{
   /* XXX: this doesn't seem to be working */
   Elf32_Shdr * sht;
   int sht_count,i;

   if( ! elf || indx < 0 )
      error_ret("bad args",NULL);

   if( ! (sht = get_sht( elf)) )  
      error_ret("can't get sht",NULL);
   sht_count = elf->sht_count;
   for(  i = 0 ; i < sht_count ; i++ ) {
      if( ( sht[i].sh_type == type ) && ( indx-- <= 0 ))
         return (&sht[i]);
   }
   return(NULL);
}

char * sh_flags2str( Elf32_Word flags )
{
   static char str[32];
   char * cur = str ;

   *cur++ = ( flags & SHF_WRITE ) ? ( 'w' ) : ( '-' );
   *cur++ = ( flags & SHF_ALLOC ) ? ( 'a' ) : ( '-' );
   *cur++ = ( flags & SHF_EXECINSTR ) ? ( 'x' ) : ( '-' );
   *cur++ = ( flags & SHF_MERGE ) ? ( 'm' ) : ( '-' );
   *cur++ = ( flags & SHF_STRINGS ) ? ( 's' ) : ( '-' );
   *cur++ = ( flags & SHF_INFO_LINK ) ? ( 'i' ) : ( '-' );
   *cur++ = ( flags & SHF_LINK_ORDER ) ? ( 'l' ) : ( '-' );
   /*
   *cur++ = ( flags & SHF_OS_NONCONFORMING ) ? ( 'o' ) : ( '-' );
   *cur++ = ( flags & SHF_GROUP ) ? ( 'g' ) : ( '-' );
   *cur++ = ( flags & SHF_TLS ) ? ( 't' ) : ( '-' );
   *cur++ = ( flags & SHF_MASKOS ) ? ( 'k' ) : ( '-' );
   *cur++ = ( flags & SHF_MASKPROC ) ? ( 'p' ) : ( '-' );
   *cur++ = ( flags & GRP_COMDAT ) ? ( 'd' ) : ( '-' );
   */

   *cur = 0;
   return(str);
}


char * sh_type2str( Elf32_Word type )
{
   char * str;

   switch(type) {
      case SHT_NULL: str = "SHT_NULL"; break;
      case SHT_PROGBITS: str = "SHT_PROGBITS"; break;
      case SHT_SYMTAB: str = "SHT_SYMTAB"; break;
      case SHT_STRTAB: str = "SHT_STRTAB"; break;
      case SHT_RELA: str = "SHT_RELA"; break;
      case SHT_HASH: str = "SHT_HASH"; break;
      case SHT_DYNAMIC: str = "SHT_DYNAMIC"; break;
      case SHT_NOTE: str = "SHT_NOTE"; break;
      case SHT_NOBITS: str = "SHT_NOBITS"; break;
      case SHT_REL: str = "SHT_REL"; break;
      case SHT_SHLIB: str = "SHT_SHLIB"; break;
      case SHT_DYNSYM: str = "SHT_DYNSYM"; break;
      case SHT_INIT_ARRAY: str = "SHT_INIT_ARRAY"; break;
      case SHT_FINI_ARRAY: str = "SHT_FINI_ARRAY"; break;
      case SHT_PREINIT_ARRAY: str = "SHT_PREINIT_ARRAY"; break;
      case SHT_GROUP: str = "SHT_GROUP"; break;
      case SHT_SYMTAB_SHNDX: str = "SHT_SYMTAB_SHNDX"; break;
      case SHT_NUM: str = "SHT_NUM"; break;
      case SHT_LOOS: str = "SHT_LOOS"; break;
      case SHT_GNU_LIBLIST: str = "SHT_GNU_LIBLIST"; break;
      case SHT_CHECKSUM: str = "SHT_CHECKSUM"; break;
      case SHT_LOSUNW: str = "SHT_LOSUNW"; break;
                       /*
      case SHT_SUNW_move: str = "SHT_SUNW_move"; break;
      */
      case SHT_SUNW_COMDAT: str = "SHT_SUNW_COMDAT"; break;
      case SHT_SUNW_syminfo: str = "SHT_SUNW_syminfo"; break;
      case SHT_GNU_verdef: str = "SHT_GNU_verdef"; break;
      case SHT_GNU_verneed: str = "SHT_GNU_verneed"; break;
      case SHT_GNU_versym: str = "SHT_GNU_versym"; break;
      /*
      case SHT_HISUNW: str = "SHT_HISUNW"; break;
      case SHT_HIOS: str = "SHT_HIOS"; break;
      */
      case SHT_LOPROC: str = "SHT_LOPROC"; break;
      case SHT_HIPROC: str = "SHT_HIPROC"; break;
      case SHT_LOUSER: str = "SHT_LOUSER"; break;
      case SHT_HIUSER: str = "SHT_HIUSER"; break;
      default: str = "unknown"; break;
   }
   return(str);
}

