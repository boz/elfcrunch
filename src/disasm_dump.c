#include"disasm_dump.h"
#include"objects.h"
#include"common.h"
#include"utils.h"
#include"functions.h"
#include"vars.h"
#include"globals.h"
#include"target.h"
#include"pltmap.h"

char * addr2objstr( addr_t addr );
char * addr2str(  addr_t addr )
{
   static char retstr[BUFSIZ];
   char * tmp;
   if( (tmp = addr2objstr( addr )) && tmp[0] )
      snprintf( retstr , sizeof(retstr) - 1 , "%s" , tmp );
   else
      snprintf( retstr , sizeof(retstr) - 1 , "0x%x" , addr );
   return(retstr);
}

char * addr2objstr( addr_t addr )
{
   char * tmp = NULL;
   elf_t * elf;
   int i;
   if( addr > 0 )
   {
      if((tmp = pltmap_name_by_addr(addr))&&tmp[0])
         return(tmp);

      if((tmp = var_name_at_addr(addr))&&tmp[0])
         return(tmp);

      if((tmp = func_name_at_addr(addr))&&tmp[0])
         return(tmp);

   }
   return(NULL);
}

char * get_operand_data_str( x86_op_t *op )
{
   static char str[BUFSIZ];
   str[0] = 0;
   if(!op) error_ret("null args",str);

   if ( op->flags & op_signed ) {
      switch ( op->datatype ) {
           case op_byte:
                   snprintf( str, BUFSIZ - 1, "0x%x", op->data.sbyte );
                   break;
           case op_word:
                   snprintf( str, BUFSIZ - 1, "0x%x", 
                             (unsigned long)op->data.sword );
                   break;
           case op_qword:
                   snprintf( str, BUFSIZ - 1, "0x%llx",
                             (unsigned long long)op->data.sqword );
                   break;
           default:
                   snprintf( str, BUFSIZ - 1, "%s",addr2str(op->data.sdword));
                   break;
      }
   }
   else
   {
      switch ( op->datatype ) {
           case op_byte:
                   snprintf( str, BUFSIZ - 1, "0x%lx", 
                             (unsigned long) op->data.byte );
                   break;
           case op_word:
                   snprintf( str, BUFSIZ - 1, "0x%lx", 
                             (unsigned long) op->data.word );
                   break;
           case op_qword:
                   snprintf( str, BUFSIZ - 1, "0x%llx",
                             (unsigned long long) op->data.sqword );
                   break;
           default:
                   snprintf( str, BUFSIZ - 1, "%s",addr2str( op->data.dword ) );
                   break;
      }
   }

   return(str);
}


int format_op( x86_op_t * op , x86_insn_t * insn , char * buf , int len )
{
   int ret;
   addr_t dst;

   if(!op ||!insn||!buf||len<1) error_ret("null args",-1);

   buf[0] = 0;
   ret = 0;
   switch( op->type ){
     case op_unused:  break;
     case op_immediate:
          snprintf(buf,len,"$%s",get_operand_data_str(op));
          break;
     case op_offset:
          snprintf(buf,len,"%s",get_operand_data_str(op));
          break;
          
     case op_relative:
          switch( op->datatype )
          {
            case op_byte:
                dst = op->data.sbyte + insn->addr + insn->size;
                break;
            case op_word:
                dst = op->data.sword + insn->addr + insn->size;
                break;
            default:
                dst = op->data.sdword + insn->addr + insn->size;
                break;
          }
          snprintf(buf,len,"%s",addr2str(dst));
          break;
     case op_absolute:
          dst = op_resolve( op , insn );
          snprintf(buf,len,"%s",addr2str(dst));
          break;
     case op_expression:
          {
             char *     cur;
             int        nwrote;
             addr_t val;
             x86_ea_t * ea;

             cur = buf;
             ea = &op->data.effective_addr;

             switch( ea->disp_size ) {
               case 1: val = (addr_t) ea->disp & 0xff;  break;
               case 2: val = (addr_t) ea->disp & 0xffff; break;
               case 4: val = (addr_t) ea->disp & 0xffffffff; break;
               default: val = 0 ; break;
             }

             if(!ea->base.name[0] && !ea->base.name[0] ) {
                   nwrote = snprintf( cur , len - (cur - buf +1) , 
                                      "*%s", addr2str(val) );
                   cur += nwrote;
             }
             else
             {

                if( ea->disp_size == 4 && ea->disp_sign && ((int)val > 0) ) {
                   nwrote = snprintf( cur , len - (cur-buf+1) , 
                                      "%s" , addr2str(val) );
                   cur += nwrote;
                }
                else if( ea->disp_size > 0 )
                {
                   if( (int)val < 0 ) {
                      nwrote = snprintf( cur , len-(cur-buf+1),"-");
                      cur += nwrote;
                      val = (addr_t)((int)val * -1);
                   }

                    nwrote =snprintf( cur, len - (cur-buf+1) , "0x%lx" , val );
                    cur += nwrote;
                }


                nwrote = snprintf( cur , len - ( cur - buf + 1) ,"(" );
                cur += nwrote;

                if( ea->base.name[0] ){
                   nwrote = snprintf( cur , len-(cur-buf+1) , 
                                      "%%%s" , ea->base.name );
                   cur += nwrote;
                }
                if( ea->index.name[0] ){

                   nwrote = snprintf( cur , len - ( cur - buf + 1) , 
                                      ",%%%s" , ea->index.name );
                   cur += nwrote;

                   if( ea->scale > 1 ) {
                      nwrote = snprintf( cur , len - ( cur - buf + 1 ) , 
                                         ",%d" , ea->scale );
                      cur += nwrote;
                   }
                }
                nwrote = snprintf( cur ,  len - ( cur - buf + 1 ) , ")" );
                cur += nwrote;
             }
             break;
          }
     case op_unknown:
          ret = -1;
          break;
     case op_register:
     default:
          ret = x86_format_operand( op , insn , buf , len , att_syntax );
          break;
   }
   return(ret);
}

int print_call( void * a , void * arg )
{
   addr_t addy = (addr_t)a;
   printf("%x,",addy);
   return(0);
}



int dump_insn( x86_insn_t * insn , void * arg)
{
   /*
   elf_t * elf;
   */
   static char buf[BUFSIZ];
   char * obj;

   if( ! insn ) error_ret("null args",-1);

   /*
   elf = (elf_t *)arg;
   */

   func_t * func;
   if((func = func_at_addr( insn->addr ))){
      printf("Function %s: 0x%x - 0x%x\n",
             OBJ_GET_NAME(func->obj),OBJ_GET_ADDR(func->obj),
             OBJ_GET_ADDR(func->obj) + OBJ_GET_SIZE(func->obj));
      printf("calls:");
      list_map_over_data( func->calls , print_call , NULL );
      printf("\n");
   }
   else
   {
      if(( obj = addr2objstr( insn->addr ) ))
         printf("<%s>\n", obj );
   }

   if( print_vma )
      printf("0x%x:\t",insn->addr);

   x86_format_mnemonic( insn , buf , MAX_OP_STRING , att_syntax );
   printf("%s\t",buf);

   if( insn->operands[op_src].type != op_unused )
   {
      format_op( &insn->operands[op_src], insn , buf , MAX_OP_STRING );
      printf("%s, ",buf);
   }
   if( insn->operands[op_dest].type != op_unused )
   {
      format_op( &insn->operands[op_dest], insn , buf , MAX_OP_STRING );
      printf("%s",buf);
   }
   if( insn->operands[op_imm].type != op_unused )
   {
      format_op( &insn->operands[op_imm], insn , buf , MAX_OP_STRING );
      printf(", %s",buf);
   }
   putchar('\n');
   return(0);
}

int test_dump( x86_insn_t * insn , void * arg )
{
   static char buf [ BUFSIZ ];

   x86_format_insn( insn , buf , BUFSIZ -1, att_syntax );
   printf("%s\n",buf);
   return(0);
}

int dump_exec_sections()
{
   elf_t * elf;
   Elf32_Shdr * shdr;
   int i;

   if(!(elf = get_elf()) ) 
      error_ret("can't get elf",-1);

   i=0;
   while( ( shdr = section_by_flag( elf , SHF_EXECINSTR , i++ )))
   {
      char * name;
      name = shstr_by_offset( elf , shdr->sh_name );
      name = name ? name : "";
      printf("\nSection \'%s\' 0x%x - 0x%x \n",name,
             shdr->sh_addr,shdr->sh_addr+shdr->sh_size);

      if( edisasm_addr_range ( elf , shdr->sh_addr , 
                               shdr->sh_addr + shdr->sh_size , 
                              (edis_callback)dump_insn , (void*)elf ) < 0 ) 
         error_ret("problem disassembling",-1);
   }
   return(0);
}

int dump_section( char * name )
{
   elf_t * elf;
   Elf32_Shdr * shdr;

   if( !name )
      return( dump_exec_sections() );

   if(!(elf = get_elf()) ) 
      error_ret("can't get elf",-1);

   if(!( shdr = section_by_name( elf , name ) ))
      error_ret("no section by that name",-1);

   if( edisasm_addr_range ( elf , shdr->sh_addr , 
                            shdr->sh_addr + shdr->sh_size , 
                           (edis_callback)dump_insn , (void*)elf ) < 0 ) 
      error_ret("problem disassembling",-1);

   return(0);
}

int dump_exec_segments()
{
   Elf32_Phdr * phdr;
   elf_t * elf;
   int i;

   if(!(elf = get_elf()))
      error_ret("can't get elf",-1);

   i=0;
   while((phdr = phdr_by_flag( elf , PF_X , i++) ))
   {
      if( phdr->p_vaddr <= 0 )
         error_ret("bad p_vaddr",-1); 

      if(!( phdr->p_type & PT_LOAD ))
         continue;

      printf("\nsegment 0x%x - 0x%x:\n",phdr->p_vaddr,phdr->p_vaddr+
                                                      phdr->p_filesz );

      if( edisasm_addr_range ( elf , phdr->p_vaddr , 
                               phdr->p_vaddr + phdr->p_filesz , 
                              (edis_callback)dump_insn , (void*)elf ) < 0 ) 
         error_ret("problem disassembling",-1);
   }
   return(0);
}


/* dump program segment by index in pht */
int dump_segment( int indx )
{
   Elf32_Phdr * phdr;
   elf_t * elf;
   if(indx<0) return(dump_exec_segments());

   if(!(elf = get_elf()))
      error_ret("can't get elf",-1);

   if(!(phdr = phdr_by_index( elf , indx )))
      error_ret("can't get phdr",-1);

   /* XXX: don't use edisasm_addr_range and allow this */
   if( phdr->p_vaddr <= 0 )
      error_ret("bad p_vaddr",-1); 

   if( !( phdr->p_flags & PF_X ))
      error_ret("not executable",-1);

   if( edisasm_addr_range ( elf , phdr->p_vaddr , 
                            phdr->p_vaddr + phdr->p_filesz , 
                           (edis_callback)dump_insn , (void*)elf ) < 0 ) 
      error_ret("problem disassembling",-1);

   return(0);
}

