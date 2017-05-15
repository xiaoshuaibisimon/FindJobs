#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int issort(void * data,int size,int esize,
           int (*compare)(const void *key1,const void * key2))
{
    char    *a = (char*)data;
    void    *key;
    int     i,
            j;

    if((key = (char *)malloc(esize)) == NULL)
        return -1;

    for(i = 1;i < size;i++)
    {
        j = i - 1;
        memcpy(key,&a[esize*i],esize);
        while(j>=0 && compare(&a[esize*j],key))
        {
            memcpy(&a[esize*(j+1)],&a[esize*j],esize);
            j--;
        }

        memcpy(&a[esize*(j+1)],key,esize);
    }

    free(key);

    return 0;
}


int main()
{

    return 0;
}
