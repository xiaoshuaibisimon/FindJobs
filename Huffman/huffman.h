/*
 * huffman.h
 *
 *  Created on: 2017年5月18日
 *      Author: Administrator
 */

#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include "pqueue.h"
#include "bitree.h"
#include "bit.h"
#include <netinet/in.h>
#include <limits.h>

typedef struct _HuffNode
{
    unsigned char symbol;
    int				freq;
}HuffNode;

typedef struct __HUffCode{

    unsigned char	used;
    unsigned short 	code;
    unsigned char 	size;
}HuffCode;


int huffman_compress(const unsigned char * original,unsigned char ** compressed,int size);
int huffman_uncompress(const unsigned char * compressed,unsigned char ** original);

#endif /* HUFFMAN_H_ */
