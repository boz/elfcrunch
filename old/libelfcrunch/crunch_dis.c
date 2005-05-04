#include"asmelflib.h"

int disasm_insn( asmelf_t * aelf , x86_insn_t * insn )
{
   static char buf[MAX_OP_STRING];
   char * append = NULL;

   if( ! aelf || !insn )
      error_ret("null args",-1);

   printf("0x%08x:\t",insn->addr);

   x86_format_mnemonic( insn , buf , MAX_OP_STRING , att_syntax );
   printf("%s\t",buf);

   if( insn->operands[op_src].type != op_unused )
   {
      x86_format_operand( &insn->operands[op_src], insn,
                          buf,MAX_OP_STRING,att_syntax );

      if( insn->type == insn_mov )
      {
         addr_t target = insn->operands[op_src].data.sdword;
         append = str_at_vma( aelf->elf , target );
      }
      printf("%s, ",buf);
   }
   if( insn->operands[op_dest].type != op_unused )
   {
      x86_format_operand( &insn->operands[op_dest], insn,
                          buf,MAX_OP_STRING,att_syntax );
      printf("%s",buf);

      if( insn->type == insn_call )
      {
         addr_t target = op_resolve( &insn->operands[op_dest],insn );
         append = plt_call_str( aelf , target );
      }
   }
   if( insn->operands[op_imm].type != op_unused )
   {
      x86_format_operand( &insn->operands[op_imm], insn,
                          buf,MAX_OP_STRING,att_syntax );
      printf(", %s",buf);
   }
   if( append )
      printf("\t-> %s",append);
   putchar('\n');
   return(0);
}

int disassemble_range( asmelf_t * aelf , addr_t start , addr_t end )
{

   char * data;
   size_t size,
          nread,
          tread;

   x86_insn_t insn;

   if( ! aelf || end < start )
      error_ret("bad args",-1);

   printf("Disassembling Range 0x%08x - 0x%08x:\n",start,end);

   size = end - start;

   if(!(data = data_at_addr( aelf->elf , start )))
      error_ret("can't get data",-1);

   tread = 0;

   while( tread < size )
   {
      if((nread = x86_disasm(data,size,start,tread,&insn))<=0)
         error_ret("problem disassembling",-1);
      tread+=nread;
      disasm_insn( aelf , & insn );
   }
   return(0);
}

int disassemble( asmelf_t * aelf )
{
   Elf32_Ehdr * ehdr;
   Elf32_Phdr * phdr;

   char * data;

   size_t size,
          nread,
          tread;

   x86_insn_t insn;

   if(!aelf || !aelf->elf )
      error_ret("null arg",-1);

   if(!(ehdr = get_ehdr( aelf->elf )))
      error_ret("can't get ehdr",-1);

   fprintf(stderr,"%s: e_entry = 0x%08x\n",__func__,ehdr->e_entry);
   if(!(phdr = get_phdr_by_addr( aelf->elf , ehdr->e_entry )))
      error_ret("can't get phdr",-1);

   size = phdr->p_filesz + phdr->p_vaddr - ehdr->e_entry;

   if(!(data = data_at_addr( aelf->elf , ehdr->e_entry )))
      error_ret("can't get data",-1);

   tread = 0;
   debug_print("size=0x%x\n",size);
   while( tread < size )
   {
      if((nread = x86_disasm(data,size - tread,ehdr->e_entry,tread,&insn))<=0)
         error_ret("problem disassembling",-1);
      tread+=nread;
      disasm_insn( aelf , & insn );
   }
   return(0);
}

