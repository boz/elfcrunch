#include"elflib.h"
#include"common.h"

int dump_ehdr( elf_t * elf )
{
   Elf32_Ehdr * ehdr; 
   if( ! elf || ! ( ehdr = get_ehdr( elf )) )
      error_ret("bad arg",-1);

   printf("Elf Header:\n");

   printf("e_ident:\t0x%x 0x%x 0x%x 0x%x , '' '%c' '%c' '%c' \n",
          ehdr->e_ident[0],
          ehdr->e_ident[1],
          ehdr->e_ident[2],
          ehdr->e_ident[3],
          ehdr->e_ident[1],
          ehdr->e_ident[2],
          ehdr->e_ident[3]
         );

   printf("e_entry:\t0x%x\n",ehdr->e_entry);

   /* XXX: wtf */
   printf("e_phoff:\t0x%x\n",ehdr->e_phoff);
   printf("e_phentsize:\t0x%x\n",(short) ehdr->e_phentsize);
   printf("e_phnum:\t0x%x\n",(short) ehdr->e_phnum);

   printf("e_shoff:\t0x%x\n",ehdr->e_shoff);
   printf("e_shentsize:\t0x%04x\n",(short) ehdr->e_shentsize);
   printf("e_shnum:\t0x%04x\n", (short) ehdr->e_shnum); 
   return(0);
}

int dump_sht( elf_t * elf )
{
   Elf32_Shdr * sht;
   size_t count,
          i;

   if( !elf )
      error_ret("null arg",-1);

   if(!(sht = get_sht(elf)))
      error_ret("can't get sht",-1);

   count = elf->sht_count;

   printf("%' '-10s" ,"addr");
   printf("%' '-10s" ,"size");
   printf("%' '-10s" ,"offset");
   printf("%' '-14s" ,"flags");
   printf("%' '-20s" ,"name");
   printf("%' '-30s" ,"type");
   printf("\n");

   for( i = 0 ; i < count ; i++ )
   {

      printf("%#-10x"  ,sht[i].sh_addr);
      printf("%#-10x"  ,sht[i].sh_size);
      printf("%#-10x"  ,sht[i].sh_offset);
      printf("%' '-14s"   , sh_flags2str(sht[i].sh_flags));
      printf("%' '-20s"  , shstr_by_offset( elf , sht[i].sh_name ));
      printf("%' '-25s"  ,sh_type2str( sht[i].sh_type ));
      putchar('\n');
   }
   return(0);
}

int dump_pht( elf_t * elf )
{
   Elf32_Phdr * pht;
   size_t count,
          i;

   if( !elf )
      error_ret("null arg",-1);

   if(!(pht= get_pht(elf)))
      error_ret("can't get pht",-1);

   count = elf->pht_count;

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
      printf("%#-10x",pht[i].p_offset);
      printf("%#-10x",pht[i].p_vaddr);
      printf("%#-10x",pht[i].p_filesz);
      printf("%#-10x",pht[i].p_memsz);
      printf("%' '-14s",pht_flags2str( &pht[i] ));
      printf("%s",pht_type2str( &pht[i] ));
      putchar('\n');
   }

   return(0);
}

int dump_symbol( elf_t * elf , Elf32_Sym * sym , char * name )
{
   char * type_str,
        * bind_str,
        * name_str;

   if( ! sym )
      error_ret("null arg",-1);

   name_str = name ? name : "unknown";
   bind_str = symbol_bind2str( sym );
   type_str = symbol_type2str( sym );

   /* value , size , bind , type , shindex , name */
   printf("%#-11x",sym->st_value);
   printf("%#-8x", sym->st_size );
   printf("%#-11s",type_str);
   printf("%#-11s",bind_str);
   printf("%#-8x",sym->st_shndx);
   printf("%#-8x",sym->st_name);
   printf("%s\n",name_str);

   return(0);
}

int sym_dump_format()
{
   printf("%' '-11s","Vma");
   printf("%' '-8s","Size");
   printf("%' '-11s","Type");
   printf("%' '-11s","Bind");
   printf("%' '-8s","ShtIndx");
   printf("%' '-8s","StrIndx");
   printf("%s","Name");
   putchar('\n');
   return(0);
}

int dump_relocs( elf_t * elf ) { return(0); }

int dump_symtab( elf_t * elf )
{
   Elf32_Sym * sym;
   int num,
       i;

   if( ! elf  )
      error_ret("bad args",-1);

   if(!(sym = get_symtab(elf)))
      error_ret("no symtab",-1);

   num = elf->symtab_len;

   printf("Symbol Table:\n");
   sym_dump_format();

   for( i = 0 ; i < num ; i++ )
      if(dump_symbol( elf , sym + i , str_by_offset(elf, sym[i].st_name ))< 0) 
         error_ret("problem dumping",-1);

   return(0);
}

int dump_dsymtab( elf_t * elf )
{
   Elf32_Sym * sym;
   int num,
       i;

   if( ! elf  )
      error_ret("bad args",-1);

   if(!(sym = get_dsymtab(elf)))
      error_ret("no symtab",-1);

   num = get_dsymcount( elf );

   printf("Dynamic Symbol Table:\n");
   printf("%' '-11s","Vma");
   printf("%' '-8s","Size");
   printf("%' '-11s","Type");
   printf("%' '-11s","Bind");
   printf("%' '-8s","ShtIndx");
   printf("%' '-8s","StrIndx");
   printf("%s","Name");
   putchar('\n');

   for( i = 0 ; i < num ; i++ )
      if(dump_symbol( elf , sym + i , 
                      dstr_by_offset( elf , sym[i].st_name  )) < 0)
         error_ret("problem dumping",-1);

   return(0);
}

int dump_dstrtab( elf_t * elf )
{
   char * str;
   if( ! elf || !( str = get_dstrtab( elf ) ) )
      error_ret("bad args", -1);
   printf("Dynamic String Table:\n");
   dump_strtab_with_data( str , elf->dstrtab_len );
   return(0);
}

int dump_shstrtab( elf_t * elf )
{
   char * str;
   if( ! elf || !( str = get_shstrtab( elf ) ) )
      error_ret("bad args", -1);
   printf("Section String Table:\n");
   dump_strtab_with_data( str , elf->shstrtab_len  );
   return(0);
}

int dump_strtab( elf_t * elf )
{
   char * str;
   if( ! elf || !( str = get_strtab( elf ) ) )
      error_ret("bad args", -1);
   printf("Section String Table:\n");
   dump_strtab_with_data( str , elf->strtab_len  );
   return(0);
}

int dump_dynamic( elf_t * elf  )
{
   if( ! elf ) 
      error_ret("bad arg",-1);

   Elf32_Dyn * dyn = get_dynamic( elf );
   if( ! dyn )
      error_ret("bad dynamic segment",-1);

   printf("Dynamic Section:\n");
   printf("%' '-20s","Type");
   printf("%' '-10s\n","Value");
   for(; dyn->d_tag != DT_NULL ; dyn++ )
   {
      printf("%' '-20s", dyn_tag2str(dyn->d_tag));
      printf("%#-10x",dyn->d_un.d_val);
      if( dyn->d_tag == DT_NEEDED )
         printf(" ->%s",dstr_by_offset( elf , dyn->d_un.d_val ) );
      putchar('\n');
   }

   return(0);
}

int dump_dt_rel( elf_t * elf ,
                 Elf32_Rel * rel , 
                 size_t size , 
                 size_t entsize )
{
   size_t num;
   int i;

   if( ! rel )
      error_ret("null arg",-1);

   num = size / entsize;

   printf("%' '-7s" ,"Index");
   printf("%' '-8s" ,"Offset");
   printf("%' '-10s","Value");
   printf("%' '-10s","Sym-Indx");
   printf("%' '-16s","Type");
   printf("%s\n","Symbol Name");

   for( i = 0 ; i < num ; i++ )
   {
      int    indx;
      char * typestr,
           * name;

      typestr = reloc_type2str( ELF32_R_TYPE(rel[i].r_info) );
      indx  = ELF32_R_SYM(rel[i].r_info);
      if(!( name = dsym_name_by_index( elf , indx )))
         name = "";

      printf("%#-7d",i);
      printf("%#-8x",i*entsize);
      printf("%#-10x",rel[i].r_offset);
      printf("%#-10x",indx);
      printf("%' '-16s",typestr);
      printf("%s\n",name);
   }
   return(0);
}

int dump_dreloc( elf_t * elf )
{
   Elf32_Rel * rel;

   if( ! elf )
      error_ret("null arg",-1);

   if(!(rel = get_dreloc( elf , DT_REL ) ) )
      error_ret("can't get rel",-1);

   printf("Dynamic Relocation Table:\n");

   dump_dt_rel( elf,
                elf->rel ,
                elf->rel_size ,
                elf->rel_entsize );
   return(0);
}


int dump_rel_plt( elf_t * elf )
{
   if( ! elf || !( get_rel_plt(elf) ))
      error_ret("bad arg",-1);

   printf("PLT Relocation Table:\n");

   if( elf->rel_plt_type == DT_REL )
      dump_dt_rel( elf ,
                   elf->rel_plt , 
                   elf->rel_plt_size , 
                   elf->rel_plt_entsize );
   else
      error_ret("ONLY DT_REL, SORRY",-1);

   return(0);
}

/* dump a single string table. */
int dump_strtab_with_data( char * data , size_t size )
{
   char *cur,
        *last;

   printf("%' '-8s","Index");
   printf("Value");

   last = cur = data;

   while( cur - data < size )
   {
      if( *cur++ == '\0' )
      {
         printf("%#-8x",last - data );
         printf("%s", strtab_printable( last ) );
         putchar('\n');
         last = cur;
      }
   }
   return(0);
}
