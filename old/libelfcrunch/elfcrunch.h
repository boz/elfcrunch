#ifndef ASMELFLIB_H
#define ASMELFLIB_H

#include "elflib.h"



/* any object in file */
typedef struct
{
   addr_t vma;
   size_t size;
   char * name;
} obj_t;

typedef struct
{
   addr_t  vma;   /* addr which gets called */
   char   *name;  /* name of function       */
} pltmap_t;

typedef struct
{
   elf_t * elf;

   addr_t plt;
   size_t plt_size;
   pltmap_t * pltmap;
   size_t     pltmap_size;

} elfcrunch_t;

/* libelfcrunch.c */

elfcrunch_t * new_elfcrunch( char * name );
int free_elfcrunch( elfcrunch_t * );

int elfcrunch_init();
int elfcrunch_cleanup();

/* asm_dis.c */
int disassemble( elfcrunch_t * aelf );

/* plt.c */
#define PLT_START_SIZE 16
#define PLT_ENTRY_SIZE 19

int    dump_plt( elfcrunch_t *  );
addr_t get_plt_by_entry_call( elfcrunch_t * );
int    dis_plt( elfcrunch_t * );

pltmap_t * get_pltmap  ( elfcrunch_t * );
char     * plt_call_str( elfcrunch_t * aelf , addr_t vma );
int        dump_pltmap ( elfcrunch_t * );

/* graph.h */

#endif /* ASMELFLIB_H */

