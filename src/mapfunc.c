#include"edisasm.h"
#include"refmap.h"
#include"functions.h"
#include"target.h"
#include"utils.h"
int gen_func_find_jmps( x86_insn_t * insn , void * arg )
{
   reflist_t * list;
   addr_t dst;

   if( insn->group != insn_controlflow )
      return(0);

   if(!((insn->type == insn_jmp )||
        (insn->type == insn_jcc )||
        (insn->type == insn_loop)))
      return(0);

   if(!(list = (reflist_t *)arg))
      error_ret("null arg",-1);

   if((dst = op_resolve( &insn->operands[op_dest] , insn ))<=0 )
      error_ret("problem resolving",-1);

   if(add_ref_map( list , insn->addr , dst )<0)
      error_ret("problem entering ref map",-1);

   return(0);
}

int gen_func_dot_map( addr_t funcaddr , addr_t start , int depth )
{

   func_t * func;
   elf_t * elf;

   refmap_t * reflist;

   if(!(func = func_at_addr(funcaddr)))
      error_ret("no func there, bitch",-1);

   if( OBJ_GET_SIZE(func->obj) == 0 )
      error_ret("maybe later we'll try to figure it out.",-1);

   if(!(elf=get_elf()))
      error_ret("can't get elf",-1);

   reflist = new_reflist();

   /* XXX: this is stupid, edis.. should be addr, size */
   if( edisasm_addr_range( elf , OBJ_GET_ADDR(func->obj) ,
                           OBJ_GET_ADDR(func->obj) + OBJ_GET_SIZE(func->obj),
                           (edis_cb)gen_func_find_jmps , reflist ) < 0 )
      error_ret("problem disassembling",-1);

   fprintf(stderr,"here's what we get:\n");

   list_map_over_data( reflist , dump_reflist , NULL );

   return(0);
}

