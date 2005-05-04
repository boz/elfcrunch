#include"xmlelflib.h"

char * elf_name = NULL;
char * inf_name = NULL;

void usage( char * name )
{
   printf("Usage: %s <file> [info file]\n",name);
   exit(1);
}

int main(int argc , char *argv[] )
{
   switch( argc ) {
     case 3:
        inf_name = argv[2];
     case 2:
        elf_name = argv[1];
        break;
     default:
        usage(argv[0]);
        break;
   }

   elf_t    * elf ;
   xmlelf_t * xelf;

   if( ! inf_name ){
      int len = strlen(elf_name) + strlen(".info") + 1;
      inf_name = malloc( len );
      snprintf( inf_name , len , "%s%s" , elf_name , ".info" );
   }

   if(!(elf = new_elf( elf_name )))
      perror_die("can't get elf",1);

   if(!(xelf = new_xmlelf( elf ,  elf_name )))
      error_die("can't get xmlelf",1);

   return(0);
}
