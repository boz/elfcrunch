#ifndef LIBUTIL_H
#define LIBUTIL_H
#include<stdlib.h>
#include<stdio.h>

#define xfree(a) if( a ) free( a )
void * xcalloc( size_t nmemb , size_t size );


#endif /* LIBUTIL_H */
