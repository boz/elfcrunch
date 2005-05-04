#include"elflib.h"
#include"common.h"

/* XXX: this should be 'sht' */
Elf32_Shdr * get_shdr( elf_t * elf )
{
   offset_t off;
   Elf32_Shdr * ret;

   if( ! elf )
      error_ret("bad arg",NULL);
   if( elf->shdr )
      return(elf->shdr);

   if( ! elf->ehdr && ! get_ehdr( elf ) )
      error_ret("can't get ehdr",NULL);
   
   off = elf->ehdr->e_shoff;
   ret = data_at_offset( elf , off );
   if( ! ret )
      error_ret("can't get data",NULL);

   elf->shdr_count = elf->ehdr->e_shnum;
   return( elf->shdr = ret );
}

int dump_shdr( elf_t * elf )
{
   Elf32_Shdr * shdr;
   size_t count,
          i;

   if( !elf )
      error_ret("null arg",-1);
   
   if(!(shdr = get_shdr(elf)))
      error_ret("can't get shdr",-1);

   count = elf->shdr_count;

   printf("addr  offset type flags size\n");
   for( i = 0 ; i < count ; i++ )
   {
      printf("0x%08x ",shdr[i].sh_addr);
      printf("0x%08x ",shdr[i].sh_offset);
      printf("0x%08x ",shdr[i].sh_type);
      printf("0x%08x ",shdr[i].sh_flags);
      printf("0x%08x ",shdr[i].sh_size);
      putchar('\n');
   }
   return(0);
}

