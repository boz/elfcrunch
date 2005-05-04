#ifndef TARGET_H
#define TARGET_H

#include"elflib.h"
#include"xmlelflib.h"

#define FILE_ENV_NAME "ELFCRUNCH_FILE"
#define INFO_EXT      "ec.xml"

/* open a new target by filename */
int open_target( char * file );
int close_target();

int open_target_info( elf_t * elf );
int free_info( xmlelf_t * xe );

/* get the elf structure for our file */
elf_t * get_elf();

/* get the information */
xmlelf_t * get_target_info(); 


#endif /* TARGET_H */

