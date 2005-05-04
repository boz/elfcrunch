#include"elflib.h"
#include"common.h"

Elf32_Phdr * get_phdr( elf_t * elf )
{
   offset_t off;
   Elf32_Phdr * ret;

   if( ! elf )
      error_ret("bad arg",NULL);
   if( elf->phdr )
      return(elf->phdr);

   if( ! elf->ehdr && ! get_ehdr( elf ) )
      error_ret("can't get ehdr",NULL);

   off = elf->ehdr->e_phoff;
   ret = data_at_offset( elf , off );
   if( ! ret )
      error_ret("can't get data",NULL);

   elf->phdr_count = elf->ehdr->e_phnum;

   return( elf->phdr = ret );
}

Elf32_Phdr * get_phdr_by_type( elf_t * elf , unsigned type , int indx)
{
   int i;

   if( ! elf || 
       ( ! elf->phdr && !get_phdr(elf) )|| 
       ( !elf->ehdr  && !get_phdr(elf) ))
      error_ret("null args",NULL);

   for( i = 0 ; i < elf->ehdr->e_phnum ; i++ )
      if( (elf->phdr[i].p_type == type) && ( indx-- >= 0 ) )
         return( elf->phdr + i );

   return(NULL);
}

Elf32_Phdr * get_phdr_by_addr( elf_t * elf , addr_t vma )
{
   Elf32_Phdr * phdr;
   int count,
       i;

   if( ! elf )
      error_ret("null arg",NULL);

   if(!(phdr = get_phdr( elf ) ))
      error_ret("can't get phdr",NULL);

   count = elf->phdr_count;

   for( i = 0 ; i < count ; i++ )
   {
      if( ( vma >= phdr[i].p_vaddr ) && 
          ( vma < phdr[i].p_vaddr + phdr[i].p_filesz ) )
         return(&phdr[i]);
   }
   return(NULL);
}

int dump_phdr( elf_t * elf )
{
   Elf32_Phdr * phdr;
   size_t count,
          i;

   if( !elf )
      error_ret("null arg",-1);

   if(!(phdr= get_phdr(elf)))
      error_ret("can't get phdr",-1);

   count = elf->phdr_count;

   printf("%' '-8s" ,"[indx]");
   printf("%' '-10s","Offset");
   printf("%' '-10s","Vma");
   printf("%' '-10s","Filesz");
   printf("%' '-10s","Memsz");
   printf("%' '-14s","Flags [rwx]");
   printf("%' '-10s","Type");

   putchar('\n');
   for( i = 0 ; i<count ; i++ )
   {
      printf("[%#04d]  ",i);
      printf("%#-10x",phdr[i].p_offset);
      printf("%#-10x",phdr[i].p_vaddr);
      printf("%#-10x",phdr[i].p_filesz);
      printf("%#-10x",phdr[i].p_memsz);
      printf("%' '-14s",phdr_flags_str( &phdr[i] ));
      printf("%s",phdr_type_str( &phdr[i] ));
      putchar('\n');
   }

   return(0);
}

char * phdr_flags_str( Elf32_Phdr * phdr )
{
   static char line[8];
   if( ! phdr )
      error_ret("null arg",NULL);

   line[0]=0;

   /* rwx */
   if( phdr->p_flags & PF_R )
      strcat(line,"x");
   else
      strcat(line,"-");

   if( phdr->p_flags & PF_W )
      strcat(line,"x");
   else
      strcat(line,"-");

   if( phdr->p_flags & PF_X )
      strcat(line,"x");
   else
      strcat(line,"-");

   return(line);
}

char * phdr_type_str( Elf32_Phdr * phdr )
{
   char * str = 0;
   if( ! phdr )
      error_ret("null arg",NULL);

   switch( phdr->p_type ){
      case PT_NULL: str = "PT_NULL"; break;
      case PT_LOAD	: str = "PT_LOAD	"; break;
      case PT_DYNAMIC: str = "PT_DYNAMIC"; break;
      case PT_INTERP: str = "PT_INTERP"; break;
      case PT_NOTE	: str = "PT_NOTE	"; break;
      case PT_SHLIB	: str = "PT_SHLIB	"; break;
      case PT_PHDR	: str = "PT_PHDR	"; break;
      case PT_TLS	: str = "PT_TLS	"; break;
      case PT_NUM	: str = "PT_NUM	"; break;
      case PT_LOOS	: str = "PT_LOOS	"; break;
      case PT_GNU_EH_FRAME: str = "PT_GNU_EH_FRAME"; break;
      case PT_GNU_STACK: str = "PT_GNU_STACK"; break;
                        
      /* case PT_LOSUNW: str = "PT_LOSUNW"; break; */
      case PT_SUNWBSS: str = "PT_SUNWBSS"; break;
      case PT_SUNWSTACK: str = "PT_SUNWSTACK"; break;
      /* case PT_HISUNW: str = "PT_HISUNW"; break; */
      case PT_HIOS	: str = "PT_HIOS	"; break;
      case PT_LOPROC: str = "PT_LOPROC"; break;
      case PT_HIPROC: str = "PT_HIPROC"; break;
      default: str = "unknown"; break;
   }
   return(str);
}

