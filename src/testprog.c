#include<stdio.h>
#include<stdlib.h>

char  str[] = "OAISDJF;ALSKJDF;ALSKDJFLASKDJF;ALSKDJF;LASKJDF;LASKDJFASLKDFJ";

int foo() {
   fprintf(stdout,"hello\n");
   return(0);
}

int func2() {
   printf("hello\n");
   printf(str);
   foo();
   exit(0);
}

int func3()
{
   func2();
   func3();
   return(0);
}

int main( int argc , char *argv[] )
{
   if( argc == 1 )
      foo();
   else
      func3();


   return(0);
}

