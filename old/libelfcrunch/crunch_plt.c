
#include"elflib.h"
#include"asmelflib.h"
#include<stdlib.h>

addr_t get_plt_by_entry_call( asmelf_t * aelf )
{
   /* XXX: only works if the first call of the entrypoint is 
    * to a plt function
    */

   char * start;
   addr_t start_addr;

   Elf32_Ehdr * ehdr;
   if( ! aelf || ! aelf->elf )
      error_ret("null arg", -1 );
   
   if( aelf->plt )
      return( aelf->plt );

   if( ! ( ehdr = get_ehdr( aelf->elf ) ) )
      error_ret("can't get ehdr",-1);

   start_addr = ehdr->e_entry;

   if( ! ( start = data_at_addr( aelf->elf , start_addr ) ) )
      error_ret("can't entry point get data", -1 );

   /* 1: get our first call */

   offset_t  offset = 0;
   size_t nread;
   x86_insn_t insn;
   addr_t dst;
   addr_t pltaddr;
   do
   {
      nread=x86_disasm( start , 36+offset , start_addr , offset , &insn );
      if( nread <= 0 )
         error_ret("can't disassemble",-1);
      offset += nread;
      /* found our first plt call */
      if( insn.type == insn_call ){
         dst = op_resolve( &insn.operands[op_dest] , &insn );
         break;
      }
   } while( offset < 256 );

   if( offset >= 256 )
      error_ret("can't find call",-1);

   if( crunch_plt_entry( aelf , dst, 0 , &pltaddr , 0 ) < 0  ) 
      error_ret("can't get plt from plt code", -1 );
   
   if( get_plt_size( aelf ) < 0 )
      error_ret("can't get size",-1);

   return(( aelf->plt = pltaddr ));
}



int get_plt_size( asmelf_t * aelf )
{
   int count;
   if( ! aelf || ! aelf->elf ) 
      error_ret("null arg",-1);

   if( aelf->plt_size )
      return( aelf->plt_size );

   if( ! get_rel_plt( aelf->elf ) )
      error_ret("can't get plt reloc",-1);

   count = aelf->elf->rel_plt_size / aelf->elf->rel_plt_entsize ;
   return( ( aelf->plt_size = PLT_START_SIZE + count * PLT_ENTRY_SIZE ));
}


int dis_plt( asmelf_t * aelf ) 
{
   addr_t plt;
   int nread,
       tread;

   x86_insn_t insn;

   if( ! aelf )
      error_ret("null arg", -1);

   printf("PLT:\n");
   /* XXX: change semantics of this */
   if( ( plt =  get_plt_by_entry_call( aelf )) < 0 )
      error_ret("can't get plt",-1);

   if( (nread = crunch_plt_header( aelf , plt , 1 )) <= 0 )
      error_ret("can't do header",-1);
   tread = nread;

   while( ( nread = crunch_plt_entry( aelf , plt + tread , 0 , 0 , 1 ) ) > 0 )
   { 
      tread += nread;
   }

   return(tread);
}

pltmap_t * get_pltmap( asmelf_t * aelf )
{
   addr_t plt;
   pltmap_t * pltmap;
   size_t     pltmap_size,
              mapcnt;

   Elf32_Rel * pltrel;

   int nread,
       tread,
       off;

   if( ! aelf || ! aelf->elf )
      error_ret("null arg",NULL);

   if((plt = get_plt_by_entry_call( aelf ) )< 0 )
      error_ret("can't get plt",NULL);

   if( (nread = crunch_plt_header( aelf , plt , 0 ) )< 0)
      error_ret("can't get header",NULL);
   tread = nread;

   if(!(pltrel = get_rel_plt( aelf->elf )))
      error_ret("can't get rel.plt",NULL);

   mapcnt = pltmap_size = 0;
   pltmap = NULL;

   while((nread = crunch_plt_entry( aelf , plt + tread , &off , 0 , 0 ))>0)
   {
      char * str;
      int pindx, /* index in plt.rel */
          sindx; /* index in symtab  */

      pindx = off / sizeof( *pltrel );

      sindx = ELF32_R_SYM(pltrel[pindx].r_info);
      
      if(!(str = sym_name_at_index( aelf->elf , sindx )))
         error_ret("can't get symbol name",NULL);

      if( ++mapcnt >= pltmap_size )
      {
         pltmap_size = (pltmap_size>0) ? (2*pltmap_size) : 2 ;
         if(!(pltmap=realloc(pltmap,pltmap_size*sizeof(*pltmap))))
            error_ret("can't allocate memory",NULL);
      }

      pltmap[mapcnt-1].vma = plt + tread;
      pltmap[mapcnt-1].name = str;
      tread += nread;
   }

   if(!(pltmap=realloc(pltmap,mapcnt*sizeof(*pltmap))))
      error_ret("can't re-alloc",NULL);

   aelf->pltmap_size = mapcnt;
   return(aelf->pltmap = pltmap);
}

char * plt_call_str( asmelf_t * aelf , addr_t vma )
{
   pltmap_t * pltmap;
   size_t num,
          i;

   if( ! aelf )
      error_ret("null arg",NULL);

   if(!(pltmap = get_pltmap(aelf)))
      error_ret("can't get pltmap",NULL);

   num = aelf->pltmap_size;

   for( i = 0 ; i < num ; i++ )
      if( vma == pltmap[i].vma )
         return( pltmap[i].name );

   return(NULL);
}

int dump_pltmap( asmelf_t * aelf)
{
   pltmap_t * pltmap;
   int num,
       i;

   if(!aelf)
      error_ret("null arg",-1);

   if(!(pltmap = get_pltmap(aelf) ))
      error_ret("can't get pltmap",-1);

   printf("Plt Map:\n");
   printf("vma     name\n");
   num = aelf->pltmap_size;
   for( i = 0 ; i < num ; i++ )
   {
      printf("0x%08x -> %s\n",
             pltmap[i].vma,
             pltmap[i].name );
   }
   return(0);
}

