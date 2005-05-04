#include"builtins.h"

int register_builtins(){
   register_show();
   register_map();
   register_set();
   register_elf_commands();
   register_help();
   return(0);
}

