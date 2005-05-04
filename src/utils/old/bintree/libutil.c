#include"libutil.h"

void * xcalloc( size_t nmemb , size_t size )
{
   void * ret;
   ret = calloc( nmemb , size );
   if( ! ret )
      perror_die("calloc()",1);
   return(ret);
}

