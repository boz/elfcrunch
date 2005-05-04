#include"elflib.h"
#include"common.h"

Elf32_Phdr * get_pht( elf_t * elf )
{
   offset_t off;
   Elf32_Phdr * ret;

   if( ! elf )
      error_ret("bad arg",NULL);

   if( elf->pht )
      return(elf->pht);

   if( ! elf->ehdr && ! get_ehdr( elf ) )
      error_ret("can't get ehdr",NULL);

   off = elf->ehdr->e_phoff;
   ret = data_at_offset( elf , off );
   if( ! ret )
      error_ret("can't get data",NULL);

   elf->pht_count = elf->ehdr->e_phnum;

   return( elf->pht = ret );
}

Elf32_Phdr * phdr_by_index( elf_t * elf , int indx )
{
   Elf32_Phdr * ret;

   if(!elf||indx<0) 
      error_ret("bad args",NULL);

   if(!(ret = get_pht( elf )))
      error_ret("can't get pht",NULL);

   if( indx >= elf->pht_count )
      error_ret("out of bounds",NULL);

   return(ret + indx);
}

Elf32_Phdr * phdr_by_type( elf_t * elf , unsigned type , int indx)
{
   int i;

   if( ! elf || 
       ( ! elf->pht && !get_pht(elf) )|| 
       ( !elf->ehdr  && !get_pht(elf) ))
      error_ret("null args",NULL);

   for( i = 0 ; i < elf->ehdr->e_phnum ; i++ )
      if( (elf->pht[i].p_type == type) && ( indx-- <= 0 ) )
         return( elf->pht + i );

   return(NULL);
}

Elf32_Phdr * phdr_by_flag( elf_t * elf , unsigned flag , int indx)
{
   int i;

   if( ! elf || 
       ( ! elf->pht && !get_pht(elf) )|| 
       ( !elf->ehdr  && !get_pht(elf) ))
      error_ret("null args",NULL);

   for( i = 0 ; i < elf->ehdr->e_phnum ; i++ )
      if( (elf->pht[i].p_flags & flag ) && ( indx-- <= 0 ) )
         return( elf->pht + i );

   return(NULL);
}

/* return the phdr that contains this address.  
 * FIXME: do they overlap?
 */
Elf32_Phdr * phdr_by_addr( elf_t * elf , addr_t vma )
{
   Elf32_Phdr * pht;
   int count,
       i;
   if( ! elf )
      error_ret("null arg",NULL);
   if(!(pht = get_pht( elf ) ))
      error_ret("can't get pht",NULL);

   count = elf->pht_count;

   for( i = 0 ; i < count ; i++ )
   {
      if( ( vma >= pht[i].p_vaddr ) && 
          ( vma < pht[i].p_vaddr + pht[i].p_filesz ) )
         return(&pht[i]);
   }
   return(NULL);
}


char * pht_flags2str( Elf32_Phdr * pht )
{
   static char line[8];
   char * cur;
   if( ! pht )
      error_ret("null arg",NULL);

   cur = line;
   *cur++ = ( pht->p_flags & PF_R ) ? 'r' : '-' ;
   *cur++ = ( pht->p_flags & PF_W ) ? 'w' : '-' ;
   *cur++ = ( pht->p_flags & PF_X ) ? 'x' : '-' ;
   *cur = 0;

   return(line);
}

char * pht_type2str( Elf32_Phdr * phdr )
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

