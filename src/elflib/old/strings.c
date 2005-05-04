#include"elflib.h"
#include"common.h"
#include<stdlib.h>
#define STR_THRESH 3

elfstr_t * get_strings( elf_t * elf )
{
   if( ! elf )
      error_ret("null arg",NULL);

   if( elf->strings )
      return( elf->strings );

   Elf32_Phdr * phdr;
   int count,
       i;

   if( ! ( phdr = get_phdr( elf ) ) )
      error_ret("can't get phdr",NULL);

   count = elf->phdr_count;
   
   elfstr_t * strings = NULL;
   size_t    strcnt   = 0;
   size_t    strsz    = 0;

   for( i = 0 ; i < count ; i++ )
   {
      char * data,
           * last,
           * cur ;
      addr_t vma;
      size_t size;
      offset_t off;

      vma  = phdr[i].p_vaddr;
      off  = phdr[i].p_offset;
      size = phdr[i].p_filesz;

      if(!(data = data_at_offset( elf , off ) ) )
         error_ret("can't get data",NULL);

      last = 0;
      cur = data;

      while( cur - data < size )
      {
         if( isgraph(*cur) || *cur==' ' || *cur=='\n' )
         {
            if( last == 0 )
               last = cur;
         }
         else if( *cur != '\0' ) /* only get null terminated ascii */
         {
            last = 0;
         }
         else
         {
            if( last != 0 )
            {
               if( cur - last >= STR_THRESH )
               {
                  ++strcnt;
                  if( strcnt >= strsz )
                  {
                     strsz = (strsz>0) ? (strsz * 2) : (2);
                     if(!(strings = realloc( strings , 
                                             strsz * sizeof(*strings))) )
                        error_ret("can't alloc memory",NULL);
                  }

                  strings[strcnt-1].addr = vma + (last - data);
                  strings[strcnt-1].off  = off + (last - data);
                  strings[strcnt-1].str  = last;
                  last = 0;
               }
               else
               {
                  last = 0;
               }
            }
         }
         ++cur;
      }
   }

   if( strcnt > 0 )
      strings = realloc( strings , strcnt*sizeof(*strings) );

   elf->string_count = strcnt;
   return( elf->strings = strings );
}

char * str_at_vma( elf_t * elf , addr_t vma )
{
   elfstr_t * strings;
   size_t  count,
           i;

   if( !elf )
      error_ret("null arg",NULL);

   if(!(strings = get_strings(elf) ) )
      error_ret("can't get strings",NULL);

   count = elf->string_count;

   for( i = 0 ; i < count ; i++ )
      if( strings[i].addr == vma )
         return( strings[i].str );

   return(NULL);
}


int dump_strings(elf_t * elf)
{
   if( ! elf )
      error_ret("null arg",-1);

   elfstr_t * strings;
   size_t  count,
           i;

   if(!(strings = get_strings(elf) ) )
      error_ret("can't get strings",-1);
   count = elf->string_count;

   printf("Strings:\n");

   printf("%' '-10s","Vma");
   printf("%' '-8s","Offset");
   printf("%s\n","String");

   for(i = 0 ; i < count ; i++ )
   {
      printf("%#-10x", strings[i].addr);
      printf("%#-8x", strings[i].off );
      printf("%s\n", strings[i].str );
   }
   return(0);
}

