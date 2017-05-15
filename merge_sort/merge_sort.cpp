#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
merge(void *data,int esize,
      int left,int mid,int right,
      int (*compare)(const void *key1,const void *key2))
{
    char *a = (char *)data,
            *m;
    int     ipos,
            jpos,
            mpos;

    ipos = left;
    jpos = mid+1;
    mpos = 0;

    if((m = (char*)malloc(esize * (right - left +1))) = NULL)
        return -1;

    while(ipos <= mid || jpos <= right)
    {
        if(ipos > mid)
        {
            while(jpos <= right)
            {
                memcpy(&m[mpos*esize],&a[jpos*esize],esize);
                jpos++;
                mpos++;
            }
            continue;
        }
        else if(jpos > right)
        {
            while(ipos <= mid)
            {
                memcpy(&m[mpos*esize],&a[ipos*esize],esize);
                ipos++;
                mpos++;
            }
            continue;
        }

        if(compare(&a[ipos*esize],&a[jpos*esize]) < 0)
        {
            memcpy(&m[mpos*esize],&a[ipos*esize],esize);
            ipos++;
            mpos++;
        }
        else
        {
            memcpy(&m[mpos*esize],&a[jpos*esize],esize);
            jpos++;
            mpos++;
        }

    }

    memcpy(&a[left*esize],m,esize*(right - left + 1));

    free(m);

    return 0;

}

int
mgsort(void *data,int size,int esize,
       int left,int right,
       int (*compare)(const void *key1,const void *key2))
{
    int mid;

    if(left < right)
    {
        mid = (int)((right - left + 1)/2);

        if(mgsort(data,size,esize,left,mid,compare) < 0)
            return -1;

        if(mgsort(data,size,esize,mid+1,right,compare) < 0)
            return -1;

        if(merge(data,esize,left,mid,right,compare) < 0)
            return -1;
    }

    return 0;
}

int main()
{
    return 0;
}
