#ifndef ELFOBJS_H
#define ELFOBJS_H
#include "elflib.h"
int get_objs_from_entry(elf_t * elf);
int get_objs_from_ehdr(Elf32_Ehdr * ehdr);

#endif /* ELFOBJS_H */
