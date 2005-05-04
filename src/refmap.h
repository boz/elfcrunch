#ifndef REFMAP_H
#define REFMAP_H

typedef struct 
{
   addr_t from; /* from where? */
   addr_t to;   /* to where?   */
} refmap_t;
#define new_reflist() 

typedef list_t reflist_t;

int add_ref_map( reflist_t * reflist , refmap_t * map );

#define NODE2FROM(a) ((refmap_t*)((a)->data.data))->from
#define NODE2TO(a)   ((refmap_t*)((a)->data.data))->to

#endif /* REFMAP_H */
