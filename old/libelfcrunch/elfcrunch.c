#include"asmelflib.h"
#include<stdlib.h>

int libdis_initialized = 0;

asmelf_t * new_asmelf( char * name )
{
   if( ! name )
      error_ret("null arg",NULL);
   asmelf_t * asmelf;

   asmelf = malloc( sizeof(*asmelf) );
   memset( asmelf , 0 , sizeof(*asmelf) );

   if(!( asmelf->elf = new_elf( name ) ) )
      error_ret("can't get elf",NULL);

   asmelf_init();
   return(asmelf);
}

int free_asmelf( asmelf_t * asmelf )
{
   if(!asmelf)
      error_ret("null arg",-1);
   if( free_elf( asmelf->elf ) < 0 )
      error_ret("problem freeing elf",-1);
   free(asmelf);
   return(0);
}

int asmelf_init()
{
   if(! libdis_initialized )
   {
      x86_init(opt_none,reporter);
      libdis_initialized = 1;
   }
   return(0);
}

int asmelf_cleanup()
{
   if( libdis_initialized )
   {
      x86_cleanup();
      libdis_initialized = 0;
   }
   return(0);
}

void dump_ops( x86_insn_t * insn )
{
   if( insn->operands[op_src].type != op_unused )
   {
      fprintf(stderr,"op_src: ");
      dump_op( &(insn->operands[op_src]) );
   }
   if( insn->operands[op_dest].type != op_unused )
   {
      fprintf(stderr,"op_dest: ");
      dump_op( &(insn->operands[op_dest]) );
   }
   if( insn->operands[op_imm].type != op_unused )
   {
      fprintf(stderr,"op_imm: ");
      dump_op( &(insn->operands[op_imm]) );
   }
}

void dump_op( x86_op_t * op )
{
   int get_val = 0;
   switch( op->type )
   {
     case op_unused:
        fprintf(stderr,"unused ");
        break;
     case op_register:
        fprintf(stderr,"register ");
        break;
     case op_immediate:
        fprintf(stderr,"immediate ");
        get_val = 1;
        break;
     case op_relative:
        fprintf(stderr,"relative ");
        get_val = 1;
        break;
     case op_absolute:
        fprintf(stderr,"absolute ");
        get_val = 1;
        break;
     case op_expression:
        fprintf(stderr,"expression ");
        break;
     case op_offset:
        fprintf(stderr,"offset ");
        get_val = 1;
        break;
     default:
        fprintf(stderr,"bad type ");
        break;
   }
   fprintf(stderr,"\t");

   if( get_val )
   {
      switch(op->datatype) {
        case op_byte:
           fprintf(stderr,"byte: 0x%x", op->data.sbyte);
           break;
        case op_word:
           fprintf(stderr,"word: 0x%x", op->data.sword);
           break;
        case op_dword:
           fprintf(stderr,"dword: 0x%x", op->data.sdword);
           break;
        default:
           fprintf(stderr,"bad datatype");
           break;
      }
   }
   fprintf(stderr,"\n");
}


