#ifndef DUMPS_H
#define DUMPS_H
#include"elflib.h"

/* dumps.c */
int dump_ehdr    ( elf_t * elf ); 
int dump_sht     ( elf_t * elf );
int dump_pht     ( elf_t * elf );
int dump_relocs  ( elf_t * elf ); 
int dump_dynamic ( elf_t * elf ); /* _DYNAMIC symbols     */
int dump_dsymtab ( elf_t * elf ); /* dynamic symbol table */
int dump_dstrtab ( elf_t * elf ); /* dynamic string table */
int dump_shstrtab( elf_t * elf ); /* section string table */
int dump_symtab  ( elf_t * elf ); /* ".symtab"            */
int dump_dreloc  ( elf_t * elf ); /* dynamic relocations  */
int dump_rel_plt ( elf_t * elf ); /* plt relocations      */

int dump_symbol  ( Elf32_Sym * sym , char * name );
int dump_strtab_with_data( char * data , size_t size );

#endif/* DUMPS_H */

