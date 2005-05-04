#ifndef GLOBALS_H
#define GLOBALS_H

#define USE_COMMANDS
extern int print_vma;

extern int do_ehdr        ,
           do_pht         ,
           do_sht         ,
#ifdef USE_COMMANDS
           do_command     ,
           use_info       ,
           save_info      ,
#endif  /* USE_COMMANDS */
           do_symtabs     ,
           do_strtabs     ,
           do_relocs      ,
           do_dyn_strtabs ,
           do_dyn_symtabs ,
           do_dyn_relocs  ,
           do_dump_funcs  ,
           do_disasm      ,
           do_vars        ,
           do_pltmap      ,
           do_disasm_seg  ,
           do_map_dmp;

extern int dis_seg;
extern int print_vma ;

#endif /* GLOBALS_H */

