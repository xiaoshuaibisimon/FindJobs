#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
issort(void * data,int size,int esize,
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

static int
compare_int(const void * int1,const void *int2)
{
    if((*(const int *)int1) > (*(const int *)int2))
        return 1;
    else if((*(const int *)int1) < (*(const int *)int2))
        return -1;
    else
        return 0;
}


static int
partition(void *data,int esize,int left,int right,
                     int(*compare)(const void *key1,const void *key2))
{
    char *  a= (char *)data;
    void    * pval,
            * tmp;
    int r[3]={0};

    if((pval = (char *)malloc(esize)) == NULL)
        return -1;

    if((tmp = (char *)malloc(esize)) == NULL)
    {
        free(pval);
        return -1;
    }


    r[0] = (rand()%(right - left + 1)) + left;
    r[1] = (rand()%(right - left + 1)) + left;
    r[2] = (rand()%(right - left + 1)) + left;

    issort(r,3,sizeof(int),compare_int);
    memcpy(pval,&a[esize*r[1]],esize);

    left--;
    right++;

    while(1)
    {
        do
        {
            right--;
        }while(compare(&a[esize*right],pval) > 0);
        do
        {
            left++;
        }while(compare(&a[esize*left],pval) < 0);

        if(left >= right )
            break;
        else
        {
            memcpy(tmp,&a[left*esize],esize);
            memcpy(&a[left*esize],&a[right*esize],esize);
            memcpy(&a[right*esize],tmp,esize);
        }
    }

    free(pval);
    free(tmp);

    return right;
}

int
qksort(void *data,int size,int esize,int left,int right,
       int(*compare)(const void *key1,const void *key2))
{
    int     mid;

    while(left < right)
    {
        if((mid = partition(data,esize,left,right,compare)) < 0)
            return -1;

        if(qksort(data,size,esize,left,mid,compare) < 0)
            return -1;

        left = mid + 1;
    }

    return 0;
}


int main()
{
    return 0;
}
