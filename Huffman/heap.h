/*
 * heap.h
 *
 *  Created on: 2017年5月18日
 *      Author: Administrator
 */

#ifndef HEAP_H_
#define HEAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef  struct _Heap
{
    int     size;
    void    (*destroy)(void * data);
    int     (*compare)(const void *key1,const void * key2);

    void    **tree;
}Heap;

#define heap_size(heap)     ((heap)->size)

void    heap_init(Heap *heap,int (*compare)(const void *key1,const void * key2),
                void (*destroy)(void * data));

void    heap_destroy(Heap *heap);

int     heap_insert(Heap *heap,const void *data);

int     heap_extract(Heap *heap,void **data);

#define heap_parent(npos)       ((int)(((npos)-1)/2))
#define heap_left(npos)         (((npos)*2)+1)
#define heap_right(npos)         (((npos)*2)+2)

#endif /* HEAP_H_ */
