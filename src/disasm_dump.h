#ifndef DISASM_DUMP_H
#define DISASM_DUMP_H
#include"edisasm.h"
#include"elflib.h"

char * addr2str(  addr_t addr );
int format_op( x86_op_t * op , x86_insn_t * insn , char * buf , int len );
int dump_insn( x86_insn_t * insn , void * arg);
int dump_section( char * );
char * addr2objstr( addr_t addr );
#endif /* DISASM_DUMP_H */
