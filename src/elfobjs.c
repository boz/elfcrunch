#include"elfobjs.h"
#include"common.h"
#include"edisasm.h"
#include"functions.h"

/* Add Elf stuff into objects() */


/*
 * get the address of a few functions & main.  This is pretty presumptious.
 */

int get_objs_from_entry(elf_t * elf)
{
   char * data;
   func_t * func;
   Elf32_Ehdr * ehdr;
   x86_insn_t insn;
   addr_t dst;
   addr_t vma;
   int nread,
       tread = 0;

   if(!elf) error_ret("null arg",-1);

   if(!(ehdr = get_ehdr(elf)))
      error_ret("can't get ehdr",-1);

   if(get_objs_from_ehdr(ehdr)<0)
      error_ret("can't get _start",-1);

   if(!(data = data_at_addr( elf , ehdr->e_entry )))
      error_ret("can't get data",-1);

   /*
   if(!(func = func_at_addr( ehdr->e_entry )))
      error_ret("this is dumb",-1);
      */

   do
   {
      nread =x86_disasm(data ,MAX_INSN_SIZE+tread, 
                        ehdr->e_entry , tread , &insn); 
      if( nread <= 0 )
         error_ret("can't get past first stage",-1);
      tread += nread;
   } while ( (tread < 512) && ( (insn.type != insn_push) || 
              (insn.operands[op_dest].type != op_immediate ) )); 

   /* this should be libc_csu_fini */
   dst = insn.operands[op_dest].data.dword;
   if(dst <= 0 ) error_ret("bad operand",-1); 
   if(!(func = func_at_addr(dst))){
      func = new_func();
      OBJ_SET_NAME(func->obj,"__libc_csu_fini");
      OBJ_SET_ADDR(func->obj,dst);
      add_func(func,0);
   }

   nread =x86_disasm(data ,MAX_INSN_SIZE+tread , 
                     ehdr->e_entry , tread , &insn); 
   if( nread <= 0 )
      error_ret("can't get past first stage",-1);
   tread += nread;

   if( (insn.type != insn_push) || 
       (insn.operands[op_dest].type != op_immediate) )
      error_ret("second push not ok",-1);

   dst = insn.operands[op_dest].data.dword;
   if(dst <= 0 )
      error_ret("bad operand",-1);
   if(!(func = func_at_addr(dst))){
      func = new_func();
      OBJ_SET_NAME(func->obj,"__libc_csu_init");
      OBJ_SET_ADDR(func->obj,dst);
      add_func( func,0 );
   }

   do
   {
      nread =x86_disasm(data ,MAX_INSN_SIZE+tread , 
                        ehdr->e_entry , tread , &insn); 
      if( nread <= 0 )
         error_ret("problem getting main",-1);
      tread += nread;
   } while ( (tread < 512) && ((insn.type != insn_push) || 
             (insn.operands[op_dest].type != op_immediate ) )); 

   if( (insn.type != insn_push) || 
       (insn.operands[op_dest].type != op_immediate) )
      error_ret("second push not ok",-1);

   dst = insn.operands[op_dest].data.dword;
   if(dst <= 0 ) error_ret("bad operand",-1);

   if(!(func = func_at_addr(dst))){
      func = new_func();
      OBJ_SET_NAME(func->obj,"main");
      OBJ_SET_ADDR(func->obj,dst);
      add_func( func ,0);
   }

   return(tread);
}

/*
 * Just get the entry point
 */

int get_objs_from_ehdr(Elf32_Ehdr * ehdr)
{
   func_t * func;
   if(!ehdr) error_ret("null arg",-1);

   if(!(func = func_at_addr( ehdr->e_entry )))
   {
      func = new_func();
      OBJ_SET_NAME(func->obj,"_start");
      OBJ_SET_ADDR(func->obj,ehdr->e_entry);
      if(add_func( func , 1 )<0)
         error_ret("problem adding",-1);
   }
   else
   {
      if(strcmp(OBJ_GET_NAME(func->obj),"_start"))
         OBJ_SET_NAME(func->obj,"_start");
   }

   return(0);
}

