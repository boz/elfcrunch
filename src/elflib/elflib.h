#ifndef ELFLIB_H
#define ELFLIB_H

#include<elf.h>
#include<stdio.h>

#define STRING_THRESH 3

typedef Elf32_Addr addr_t;
typedef Elf32_Off  offset_t;

typedef struct
{
   addr_t    addr;
   offset_t  off;
   char     *str;
} elfstr_t;

typedef struct
{
   char * name;
   void * data;
   int    fd;
   size_t size;

   Elf32_Ehdr * ehdr;

   Elf32_Shdr * sht;
   int sht_count;

   Elf32_Phdr * pht;
   int pht_count;

   Elf32_Dyn * dynamic; /* dynamic segment */

   char      * shstrtab; /* section string table */
   size_t  shstrtab_len; /* length in bytes  */

   char      * dstrtab;  /* dynamic string table */ 
   size_t  dstrtab_len;  /* length in bytes  */

   char      * strtab;   /* string table (".strtab" section)*/
   size_t  strtab_len;   /* length in bytes  */

   Elf32_Sym * dsymtab;  /* dynamic symbol table  */
   size_t  dsymtab_len;  /* number of entries XXX */

   Elf32_Sym *  symtab;  /* dynamic symbol table  */
   size_t   symtab_len;  /* number of entries XXX */

   void * rel_plt;         /* plt relocation table */
   size_t rel_plt_size;    /* number of bytes      */
   size_t rel_plt_entsize; /* size of each entry   */
   unsigned rel_plt_type;  /* DT_REL or DT_RELA    */

   Elf32_Rel * rel;  /* dynamic relocation table */
   size_t rel_size;
   size_t rel_entsize;

   Elf32_Rela * rela;  /* dynamic relaocation table */
   size_t relasz;
   size_t relaent;

} elf_t ;

/* elflib.c */
elf_t *new_elf( char *  );
int    free_elf( elf_t * arg );
int    open_elf( elf_t * ret , char * name );
void  *data_at_offset( elf_t * elf , offset_t offset );
void  *data_at_addr( elf_t * elf , addr_t vma );
size_t fgetsize( int fd );

/* ehdr.c */
Elf32_Ehdr * get_ehdr( elf_t * elf );
int          verify_elf( elf_t * elf );

/* sht.c */
int has_sht( elf_t * elf );
Elf32_Shdr * get_sht( elf_t * elf );
Elf32_Shdr * section_by_index( elf_t * elf , int indx );
Elf32_Shdr * section_by_name ( elf_t * elf , char * name );
Elf32_Shdr * section_by_type ( elf_t * elf , Elf32_Word type , int indx );
Elf32_Shdr * section_by_flag ( elf_t * elf , int flag , int indx );
char * sh_flags2str( Elf32_Word flags );
char * sh_type2str( Elf32_Word type );

/* pht.c */
Elf32_Phdr * get_pht( elf_t * elf );
Elf32_Phdr * phdr_by_index( elf_t * elf , int indx );
Elf32_Phdr * phdr_by_type( elf_t * elf , unsigned type , int indx);
Elf32_Phdr * phdr_by_flag( elf_t * elf , unsigned flag , int indx);
Elf32_Phdr * phdr_by_addr( elf_t * elf , addr_t vma );
char * pht_flags2str( Elf32_Phdr * pht );
char * pht_type2str( Elf32_Phdr * phdr );

/* strtabs.c */
char * strtab_printable( char * str );
char * get_shstrtab( elf_t * elf );
char * get_dstrtab( elf_t * elf );
char * get_strtab( elf_t * elf );
char * shstr_by_offset( elf_t * elf , size_t off );
char * dstr_by_offset( elf_t * elf , size_t off );
char * str_by_offset( elf_t * elf , size_t off );

/* sym.c */
char * symbol_bind2str( Elf32_Sym * sym );
char * symbol_type2str( Elf32_Sym * sym );

/* dsymtab.c */
Elf32_Sym * get_dsymtab( elf_t * elf );
Elf32_Sym * dsym_by_index( elf_t * elf , int indx );
char * dsym_name_by_index( elf_t * elf , int indx );
Elf32_Sym * dsym_by_type( elf_t * elf , int type , int indx );
int    get_dsymcount( elf_t * elf );

/* symtab.c */
Elf32_Sym * get_symtab( elf_t * elf );
Elf32_Sym * sym_by_index( elf_t * elf , int indx );
Elf32_Sym * sym_by_type ( elf_t * elf , int type , int indx );
char * sym_name_by_index( elf_t * elf , int indx );
char * sym_name_by_value( elf_t * elf , int val , int indx );

/* dynamic.c */
Elf32_Dyn * get_dynamic( elf_t * elf );
Elf32_Dyn * dyn_sym_by_type( elf_t * elf , int type , int indx );
char * dyn_tag2str( int tag );

/* reloc.c */
void      * get_dreloc    ( elf_t * elf , unsigned  type );
Elf32_Rel * get_dt_rel    (elf_t * elf);
char      * reloc_type2str( int type );
int       rel_max_dsym_indx(  elf_t * elf );

/* rel_plt.c */
void * get_rel_plt( elf_t * elf );
int rel_plt_max_dsym_indx( elf_t * elf );

#endif /* ELFLIB_H */
