#ifndef PLT_H
#define PLT_H
#include "target.h"

#define PLT_START_SIZE 16
#define PLT_ENTRY_SIZE 19

addr_t get_plt(int * size);
addr_t get_plt_by_sht( int * size );
addr_t get_plt_by_call( elf_t * elf , addr_t vma );

int crunch_plt_header( char * plt , addr_t vma );

int    crunch_plt_entry( char * entry      , 
                         addr_t vma        , /* code calls this vma */
                         int    * reloff   , /* offset pushed */
                         addr_t * plt      );/* address of plt start */
#endif /* PLT_H */

