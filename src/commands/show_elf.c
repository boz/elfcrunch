#include"elflib.h"
#include"commands.h"
#include"common.h"
#include"target.h"

int show_ehdr( char **argv )
{
   if( *argv ) error_ret("bad args",-1);
   dump_ehdr( get_elf() );
   return( COMMAND_SUCCESS );
}

cmd_t show_ehdr_cmd = {
   "ehdr",
   "show ehdr table",
   show_ehdr
};

int show_pht( char **argv )
{
   if( *argv ) error_ret("bad args",-1);
   dump_pht( get_elf() );
   return( COMMAND_SUCCESS );
}

cmd_t show_pht_cmd = {
   "phdr",
   "Show Program Header Table",
   show_pht
};

int show_sht( char **argv )
{
   if( *argv ) error_ret("bad args",-1);
   dump_sht( get_elf() );
   return( COMMAND_SUCCESS );
}

cmd_t show_sht_cmd = {
   "sht",
   "show sht table",
   show_sht
};

int show_strtab( char **argv )
{
   if( *argv ) error_ret("bad args",-1);
   dump_strtab( get_elf() );
   return( COMMAND_SUCCESS );
}

cmd_t show_strtab_cmd = {
   "strtab",
   "show string table",
   show_strtab
};

int show_shstrtab( char **argv )
{
   if( *argv ) error_ret("bad args",-1);
   dump_shstrtab( get_elf() );
   return( COMMAND_SUCCESS );
}

cmd_t show_shstrtab_cmd = {
   "shstrtab",
   "show section string table",
   show_shstrtab
};

int show_dstrtab( char **argv )
{
   if( *argv ) error_ret("bad args",-1);
   dump_dstrtab( get_elf() );
   return( COMMAND_SUCCESS );
}

cmd_t show_dstrtab_cmd = {
   "dstrtab",
   "show dynamic string table",
   show_dstrtab
};


int show_symtab( char **argv )
{
   if( *argv ) error_ret("bad args",-1);
   dump_symtab( get_elf() );
   return( COMMAND_SUCCESS );
}

cmd_t show_symtab_cmd = {
   "symtab",
   "show symtab table",
   show_symtab
};

int show_dsymtab( char **argv )
{
   if( *argv ) error_ret("bad args",-1);
   dump_dsymtab( get_elf() );
   return( COMMAND_SUCCESS );
}

cmd_t show_dsymtab_cmd = {
   "dsymtab",
   "show dsymtab table",
   show_dsymtab
};

int show_dynamic( char **argv )
{
   if( *argv ) error_ret("bad args",-1);
   dump_dynamic( get_elf() );
   return( COMMAND_SUCCESS );
}

cmd_t show_dynamic_cmd = {
   "dynamic",
   "show dynamic table",
   show_dynamic
};

int show_rel_plt( char **argv )
{
   if( *argv ) error_ret("bad args",-1);
   dump_rel_plt( get_elf() );
   return( COMMAND_SUCCESS );
}

cmd_t show_rel_plt_cmd = {
   "rel_plt",
   "show rel_plt table",
   show_rel_plt
};

int show_dreloc( char **argv )
{
   if( *argv ) error_ret("bad args",-1);
   dump_dreloc( get_elf() );
   return( COMMAND_SUCCESS );
}

cmd_t show_dreloc_cmd = {
   "dreloc",
   "show relocation table",
   show_dreloc
};

int register_elf_commands()
{
   register_command( "/show/" , &show_ehdr_cmd );
   register_command( "/show/" , &show_pht_cmd );
   register_command( "/show/" , &show_sht_cmd );
   register_command( "/show/" , &show_strtab_cmd );
   register_command( "/show/" , &show_shstrtab_cmd );
   register_command( "/show/" , &show_dstrtab_cmd );
   register_command( "/show/" , &show_symtab_cmd );
   register_command( "/show/" , &show_dsymtab_cmd );
   register_command( "/show/" , &show_dynamic_cmd );
   register_command( "/show/" , &show_rel_plt_cmd );
   register_command( "/show/" , &show_dreloc_cmd );
}

