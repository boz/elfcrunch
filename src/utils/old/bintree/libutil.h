#ifndef LIBUTIL_H
#define LIBUTIL_H
#include<stdlib.h>
#include<stdio.h>

#define xfree(a) if( a ) free( a )
void * xcalloc( size_t nmemb , size_t size );

#define perror_die( a , b ) { perror( a ); exit(b); }

#define error_ret( str , ret ) {\
   fprintf(stderr,"%s/%s(): %s\n",\
           __FILE__ ,__func__ , str );\
   return( ret );\
}

#endif /* LIBUTIL_H */
