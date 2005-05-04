#ifndef COMMON_H
#define COMMON_H
#include<stdio.h>
#include<stdlib.h>

#define MAX_NAME 512

/* 
 * Common definitions & functions 
 */

/* error macros */
#define error_ret( str , ret ) do { \
   fprintf(stderr,"%s/%s(): %s\n", \
           __FILE__ ,__func__ , str );\
   return( ret );\
} while(0);

#define perror_die( str , ret ) do { \
   fprintf(stderr,"%s/%s(): \n", \
           __FILE__ ,__func__ );\
           perror(str); exit(ret); \
} while(0);

#define  error_die( str , ret ) do { \
   fprintf(stderr,"%s/%s(): $s \n", \
           __FILE__ ,__func__ , str );\
           exit(ret); \
} while(0);

#define debug_print(a,b ...) do{ fprintf(stderr,"%s(): ",__func__);\
   fprintf(stderr,a, b## ); \
} while(0);

#define error_print( a ) do{ fprintf(stderr,"%s(): ",__func__);\
   fprintf(stderr,"%s\n", a ); \
} while(0);

#endif /* COMMON_H */

