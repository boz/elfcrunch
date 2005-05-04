#ifndef EDISASM_H
#define EDISASM_H

#define LIBDISASM_NO_COMPAT
#include<libdis.h>

#include"elflib.h"
#include"common.h"

typedef DISASM_CALLBACK edis_callback;
typedef DISASM_CALLBACK edis_cb; /* XXX: should change all ... lazy */

int disasm_finish();
int disasm_init();

void reporter( enum x86_report_codes code, void *arg );

addr_t op_resolve( x86_op_t * op , x86_insn_t * insn );

int edisasm_addr_range( elf_t *elf,
                        addr_t start, 
                        addr_t end, 
                        edis_callback cb,
                        void *udata  );

#endif /* EDISASM_H */
