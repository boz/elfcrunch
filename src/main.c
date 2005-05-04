/* main program for elfcrunch */

#include "elflib.h"
#include "target.h"   /* handle target file */
#include "common.h"
#include "functions.h"
#include <stdlib.h>
#include <unistd.h>

#define USE_COMMANDS

#ifdef USE_COMMANDS
#include "builtins.h"
#include "commands.h"
#endif /* USE_COMMANDS */

char *filename   = 0;  /* name of target file */
char *section_name = 0;

/* globals.h */
int do_ehdr        = 0,
    do_pht         = 0,
    do_sht         = 0,
#ifdef USE_COMMANDS
    do_command     = 0,
    use_info       = 0,
    save_info      = 0,
#endif /* USE_COMMANDS */
    do_symtabs     = 0,
    do_strtabs     = 0,
    do_relocs      = 0,
    do_dyn_strtabs = 0,
    do_dyn_symtabs = 0,
    do_dyn_relocs  = 0,
    do_dump_funcs  = 0,
    do_disasm      = 0,
    do_vars        = 0,
    do_pltmap      = 0,
    do_disasm_seg  = 0,
    do_map_dmp     = 0;

int dis_seg = -1;

int print_vma = 1;
void usage( char * name )
{
   printf("Usage: %s [option ...] file \n",name);
   printf("Options:\n");
   printf("-f  file(use w/ -c )\n");
   printf("-e  elf header\n");
   printf("-p  program header table\n");
   printf("-s  section header table\n");
#ifdef USE_COMMANDS
   printf("-c  <command> elfcrunch command\n");
   printf("-i  use info\n");
   printf("-I  save info\n");
#endif /* USE_COMMANDS */
   printf("-t  symbol table(s)\n");
   printf("-T  dynamic symbol table\n");
   printf("-g  string table(s)\n");
   printf("-G  dynamic string table\n");
   printf("-r  relocation entries\n");
   printf("-R  dynamic relocation entries\n");
   printf("-F  dump functions\n");
   printf("-M  print function map\n");
   printf("-D <name> disassemble section (default all executable)\n");
   printf("-d <num>  disassemble segment '                      '\n");
   printf("-v  dump variables\n");
   printf("-P  dump plt map\n");
   exit(1);
}

/* return index at which there are options to parse */
int do_options( int argc , char **argv )
{
   extern char * optarg;
   extern int optind;
   int c;
   static char * optstring = "f:epsciIgtrGTRFMD::d::vP";

   while((c=getopt( argc , argv , optstring )) != -1 )
   {
      switch( c )
      {
        case 'f':  filename = optarg ; break;
        case 'e':  do_ehdr        = 1; break;
        case 'p':  do_pht         = 1; break;
        case 's':  do_sht         = 1; break;
#ifdef USE_COMMANDS
        case 'c':  do_command     = 1; break;
        case 'i':  use_info       = 1; break;
        case 'I':  save_info      = 1; break;
#endif /* USE_COMMANDS */
        case 'g':  do_strtabs     = 1; break;
        case 't':  do_symtabs     = 1; break;
        case 'r':  do_relocs      = 1; break;
        case 'G':  do_dyn_strtabs = 1; break;
        case 'T':  do_dyn_symtabs = 1; break;
        case 'R':  do_dyn_relocs  = 1; break;
        case 'F':  do_dump_funcs  = 1; break;
        case 'M':  do_map_dmp     = 1; break;
        case 'D':  do_disasm      = 1; 
                   section_name = optarg ;break;
        case 'd':  do_disasm_seg  = 1; 
                   dis_seg  = optarg ? atoi(optarg) : -1 ;break;
        case 'v':  do_vars        = 1; break;
        case 'P':  do_pltmap      = 1; break;
        default : usage(*argv);
      }
   }
   return(optind);
}

int dump_strtabs( elf_t * elf )
{
   printf("Section String Table:\n");
   dump_shstrtab(elf);
   dump_strtab(elf);
   return(0);
}

int dump_dyn_relocs( elf_t * elf )
{
   dump_dreloc(elf);
   dump_rel_plt(elf);
   return(0);
}

int dump_function_map()
{
   func_t * func;
   /*
   dump_functions();
   */
   if(!( func = func_by_name("main",0)))
      error_ret("can't get main's address",-1);

   /*
   dump_func_map( OBJ_GET_ADDR(func->obj) , -1 );
   */
   dump_dot_map( OBJ_GET_ADDR(func->obj),-1 );


   return(0);
}


int main( int argc , char *argv[] )
{
   int argindx;
   
   argindx = do_options( argc , argv );

   if( ! filename )
      if( !( filename = *(argv + argindx)))
         if( !( filename = getenv(FILE_ENV_NAME) ))
            usage(argv[0]);

   open_target( filename );

   if( do_ehdr    ) dump_ehdr( get_elf() );
   if( do_pht     ) dump_pht ( get_elf() );
   if( do_sht     ) dump_sht ( get_elf() );
   if( do_strtabs ) dump_strtabs ( get_elf() );
   if( do_symtabs ) dump_symtab  ( get_elf() );
   if( do_relocs  ) dump_relocs  ( get_elf() );
   if( do_dyn_strtabs ) dump_dstrtab( get_elf() );
   if( do_dyn_symtabs ) dump_dsymtab( get_elf() );
   if( do_dyn_relocs  ) dump_dyn_relocs ( get_elf() );
   if( do_disasm      ) dump_section( section_name );
   if( do_disasm_seg  ) dump_segment( dis_seg );
   if( do_dump_funcs  ) dump_functions();
   if( do_map_dmp     ) dump_function_map();
   if( do_vars        ) dump_vars();
   if( do_pltmap      ) dump_pltmap();

#ifdef USE_COMMANDS
   if( do_command ) {
      register_builtins();
      run_command( argv + argindx );
   }
#endif /* USE_COMMANDS */

   close_target();

   return(0);
}


