#include"target.h"
#include"common.h"
#include"globals.h"
#include"elfobjs.h"

/* main.c */
extern int use_info;
extern int save_info;

/* elf file for the whole prog */
static elf_t * target_elf = 0;
elf_t * get_elf() { return target_elf; }

static xmlelf_t * target_info = 0;
xmlelf_t * get_target_info() { return target_info; }

int open_target( char * file )
{
   if(!file)
      error_die("null arg",1);

   /* there's only 1 target */
   if( target_elf ) 
      free_elf( target_elf );

   if(!(target_elf = new_elf(file)))
      error_ret("can't open elf",-1);

#ifdef USE_COMMANDS
   /* check for info saved in xml file */
   if( use_info )
      if(open_target_info( target_elf ) < 0 )
         error_ret("problem getting info",-1);
#endif /* USE_COMMANDS */

   disasm_init();
   crunch_pltmap();
   gather_funcs_by_symtab( get_elf() );
   gather_funcs_by_section(NULL);
   get_objs_from_entry(get_elf());
   gather_vars( get_elf() );
   sync_func_map();

#if 0
   if( do_disasm || do_pltmap ||do_disasm_seg ||do_map_dmp){
      crunch_pltmap();
   }

   if( do_dump_funcs || do_disasm || do_disasm_seg || do_map_dmp ){
      gather_funcs_by_symtab( get_elf() );
      gather_funcs_by_section(NULL);
      get_objs_from_entry(get_elf());
   }
   if( do_vars || do_disasm || do_disasm_seg || do_map_dmp || do_map_dmp )
      gather_vars( get_elf() );
   sync_func_map();
#endif

   return(0);
}

int close_target()
{
   if( target_elf ) {
      free_elf(target_elf);
      target_elf = NULL;
   }
#ifdef USE_COMMANDS
   if( target_info ) {
      if( save_info ) 
         xmlelf_write_info( target_info );
      free_info( target_info );
      target_info = NULL;
   }
#endif /* USE_COMMANDS */

   if( do_disasm )
      disasm_finish();
   return(0);
}

int open_target_info( elf_t * elf )
{
   char * infoname;
   size_t size;
   if(!elf->name) error_ret("null elf name",-1);

   /* XXX: always save in current dir?  */

   size = strlen(elf->name) + strlen(INFO_EXT) + 4;
   infoname = malloc( size );
   snprintf( infoname ,  size-1  , "%s.%s" , elf->name , INFO_EXT );

   if( target_info )
      free_info( target_info );

   if(!(target_info = new_xmlelf( elf , infoname ) ) ){
      free(infoname);
      error_ret("can't get xmlelf",-1);
   }
   free(infoname);
}

int free_info( xmlelf_t * xe )
{
   free_xmlelf(xe);
   return(0);
}

