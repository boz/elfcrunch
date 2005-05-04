#include"plt.h"
#include"edisasm.h"


addr_t get_plt(int * size)
{
   addr_t ret;
   if((ret = get_plt_by_sht(size))>0)
      return(ret);

   elf_t * elf;
   Elf32_Ehdr * ehdr;

   if(!(elf = get_elf()))
      error_ret("can't get elf",-1);

   if(!(ehdr = get_ehdr(elf)))
      error_ret("can't get ehdr",-1);

   if((ret = get_plt_by_call( elf , ehdr->e_entry ) )>0)
      return(ret);

   return(-1);
}

   
/* 
 * Return the beginning of the plt section 
 */
addr_t get_plt_by_sht( int * size )
{
   Elf32_Shdr * shdr;
   elf_t * elf;

   if(!(elf = get_elf()))
      error_ret("no elf",-1);

   if(!(shdr = section_by_name( elf , ".plt" )))
      return( -1 );

   if(size)
      *size = shdr->sh_size;

   return( shdr->sh_addr );
}

/*
 * look at and below an address for a call to the plt section.
 * Then use that call address to find the top of the plt.
 */
addr_t get_plt_by_call( elf_t * elf , addr_t vma )
{
   char * data;
   x86_insn_t insn;
   size_t insns_read = 0;

   if(!elf) error_ret("null args",-1);

   if(!(data = data_at_addr( elf , vma ) ))
      error_ret("can't get data",-1);

   int nread,
       tread = 0;

   nread = x86_disasm( data , MAX_INSN_SIZE , vma , tread , &insn );
   if( nread <= 0 ) error_ret("problem disasembling",-1);
   tread += nread;

   while( insn.type != insn_call )
   {
      if( ++insns_read >= 20 )
         break;

      nread = x86_disasm( data , MAX_INSN_SIZE , vma , tread , &insn );
      if( nread <= 0 ) error_ret("problem disasembling",-1);
      tread += nread;

   }

   if( insn.type != insn_call )
      return(-1);

   addr_t dst,plt;
   dst = op_resolve( &insn.operands[op_dest] , & insn );

   if(!(data = data_at_addr( elf , dst )))
      error_ret("can't get data for plt call",-1);

   if( crunch_plt_entry( data , dst , NULL , &plt ) == 0 )
      return(-1); /* not plt call */
   return( plt );
}

int crunch_plt_header( char * plt , addr_t vma )
{
   int tread,
       nread;

   x86_insn_t insn;

   if( !plt || vma <= 0 ) error_ret("null arg",-1);

   tread = 0;
   if(( nread = x86_disasm( plt , PLT_START_SIZE , vma , tread , &insn ))<=0)
      error_ret("can't disasm",-1);
   if( insn.type != insn_push )
      error_ret("first insn not push",-1);
   tread += nread;

   if(( nread = x86_disasm( plt , PLT_START_SIZE , vma , tread , &insn ))<=0)
      error_ret("can't disasm",-1);
   if( insn.type != insn_jmp )
      error_ret("first insn not jmp",-1);
   tread += nread;

   while( tread < PLT_START_SIZE )
   {
      if(( nread = x86_disasm( plt , 
                               PLT_START_SIZE , 
                               vma , 
                               tread , 
                               &insn ))<=0)
         error_ret("can't disasm",-1);
      tread += nread;
   }
   return(tread);
}


/* return number of bytes read */
int    crunch_plt_entry( char * entry      , 
                         addr_t vma        , /* code calls this vma */
                         int    * reloff   , /* offset pushed */
                         addr_t * plt      ) /* address of plt start */
{
   x86_insn_t insn;
   int nread;
   int offset;

   /* jmp *got */
   if((nread = x86_disasm( entry , 64 ,  vma , 0 , &insn ) )<= 0 )
      error_ret("can't disasm",-1);
   offset = nread;
   if( insn.type != insn_jmp )
      return(0);

   /* push offset_in_rel_plt */
   if((nread = x86_disasm( entry , 64 ,  vma , offset , &insn ) )<= 0 )
      error_ret("can't disasm",-1);
   offset += nread;
   if( insn.type != insn_push )
      return(0);

   if( reloff )
   {
      offset_t off;
      off = insn.operands[op_dest].data.dword ;
      *reloff  =  off;
   }

   /* jmp top_of_plt */
   if((nread = x86_disasm( entry , 64 ,  vma , offset , &insn ) )<= 0 )
      error_ret("can't disasm",-1);
   offset += nread;
   if( insn.type != insn_jmp )
      return(0);

   if( plt )
   {
      addr_t dst;
      dst = op_resolve( &insn.operands[op_dest] , & insn );
      *plt = dst;
   }
   return( offset );
}

