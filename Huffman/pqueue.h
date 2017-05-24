/*
 * pqueue.h
 *
 *  Created on: 2017年5月18日
 *      Author: Administrator
 */

#ifndef PQUEUE_H_
#define PQUEUE_H_
#include "heap.h"

typedef Heap PQueue;

#define pqueue_init     heap_init
#define pqueue_destroy  heap_destroy
#define pqueue_insert   heap_insert
#define pqueue_extract  heap_extract
#define pqueue_peek(queue)      ((pqueue)->tree == NULL ? NULL:(pqueue)->tree[0])
#define pqueue_size     heap_size

typedef struct Parcel_ {

int                priority;

} Parcel;


#endif /* PQUEUE_H_ */
