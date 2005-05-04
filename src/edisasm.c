/*
 * Routines to interface libdisasm.
 */

#include"edisasm.h"

int libdis_initialized = 0;

int disasm_init()
{
   if( !libdis_initialized ) {
      if(x86_init( opt_none , reporter )<0)
         error_ret("can't initialize x86",-1);
      libdis_initialized = 1;
   }
   return(0);
}

int disasm_finish()
{
   if( libdis_initialized ) {
      if( x86_cleanup() < 0 ) 
         error_ret("cleanup failed",-1);
      libdis_initialized = 0;
   }
   return(0);
}

/* ripped from man page */
void reporter( enum x86_report_codes code, void *arg ) {
   char * str = NULL;
   switch ( code ) {
     case report_disasm_bounds:
        str = "Attempt to disassemble RVA beyond end of buffer";
        break;
     case report_insn_bounds:
        str   =  "Instruction  at  RVA  extends  beyond buffer";
        break;
     case report_invalid_insn:
        str = "Invalid opcode at RVA";
        break;
     case report_unknown:
        str = "Unknown Error";
        break;
   }
   fprintf(stderr, "ERROR \'%s:\' %p", str, arg);
}

/* resolve and addresss from jmp/call offset */
addr_t op_resolve( x86_op_t * op , x86_insn_t * insn )
{
   addr_t retval;
   if ( op->type == op_absolute || op->type == op_offset ) {
      retval = op->data.sdword; 
   } else if (op->type == op_relative ){
      switch( op->datatype ) {
        case op_byte: 
           retval = insn->addr + insn->size + op->data.sbyte;
           break;
        case op_word:
           retval = insn->addr + insn->size + op->data.sword;
           break;
        case op_dword:
           retval = insn->addr + insn->size + op->data.sdword;
           break;
        default:
           retval = 0;
           break;
      }
   }
   return(retval);
}

int edisasm_addr_range( elf_t *elf,
                        addr_t start, 
                        addr_t end, 
                        edis_callback cb,
                        void *udata  )
{

   char * data;
   int    ret;

   if( ! elf || (start <= 0) || ( end < start ) )
      error_ret("bad args",-1);

   if(!( data = data_at_addr( elf , start )))
      error_ret("can't get data\n",-1);

   ret = x86_disasm_range( data , start , 0 , ( end - start ) , cb , udata );

   if( ret < 0 )
      error_ret("libdis failed",-1);
   return(ret);

}

