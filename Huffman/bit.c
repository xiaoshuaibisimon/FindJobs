/*
 * bit.c
 *
 *  Created on: 2017年5月18日
 *      Author: Administrator
 */
#include "bit.h"

int
bit_get(const unsigned char * bits,int pos)
{
    int i = 0;
    unsigned char mask = 0x80;

    for(i = 0; i < (pos % 8);i++)
    {
        mask = mask >> 1;
    }

    return ((mask & (bits[(int) (pos/8)])) == mask ? 1:0);
}
void
bit_set(unsigned char * bits,int pos,int state)
{
    int i = 0;
    unsigned char mask = 0x80;

    for(i = 0; i < (pos % 8);i++)
    {
        mask = mask >> 1;
    }

    if(state)
        bits[(int)pos/8] |= mask;
    else
        bits[(int)pos/8] &= ~mask;

    return ;
}
void
bit_xor(const unsigned char *bits1,const unsigned char *bits2,unsigned char *bitsx,int size)
{
    int i = 0;

    for(i = 0;i < size;i++)
    {
        if(bit_get(bits1,i) != bit_get(bits2,i))
            bit_set(bitsx,i,1);
        else
            bit_set(bitsx,i,0);
    }
    return ;
}
void
bit_rot_left(unsigned char *bits,int size,int count)
{
    int 	i=0,
            j = 0,
            fbit,
            lbit;

    if(size > 0)
    {
        for(j = 0; j < count;j++)
        {
            for(i = 0;i <= (size-1)/8;i++)
            {
                lbit = bit_get(&bits[i],0);
                if(i == 0)
                    fbit = lbit;
                else
                    bit_set(&bits[i-1],7,lbit);

                bits[i] = bits[i]<<1;
            }

            bit_set(bits,size-1,fbit);
        }
    }

    return ;

}
