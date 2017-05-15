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

    //归并的时候需要多一倍的空间
    if((m = (char*)malloc(esize * (right - left +1))) = NULL)
        return -1;

    //只要两个区间的任何一个还有元素需要放入归并序列里就继续归并过程
    while(ipos <= mid || jpos <= right)
    {
        if(ipos > mid)
        {//左区间已经全部放入归并序列--右边还有元素剩余
            while(jpos <= right)
            {
                memcpy(&m[mpos*esize],&a[jpos*esize],esize);
                jpos++;
                mpos++;
            }
            continue;
        }
        else if(jpos > right)
        {//右区间已经全部放入归并序列--左边还有元素剩余
            while(ipos <= mid)
            {
                memcpy(&m[mpos*esize],&a[ipos*esize],esize);
                ipos++;
                mpos++;
            }
            continue;
        }

        //两个区间都还需要放入归并队列
        if(compare(&a[ipos*esize],&a[jpos*esize]) < 0)//左区间元素小
        {
            memcpy(&m[mpos*esize],&a[ipos*esize],esize);
            ipos++;//哪个区间放入归并序列--记录该区间变化位置即可
            mpos++;
        }
        else//右区间元素小
        {
            memcpy(&m[mpos*esize],&a[jpos*esize],esize);
            jpos++;
            mpos++;
        }

    }//两个区间所有元素都放入了归并序列

    //将归并序列复制到原始序列对应的位置
    memcpy(&a[left*esize],m,esize*(right - left + 1));

    //回收内存
    free(m);

    return 0;

}

int
mgsort(void *data,int size,int esize,
       int left,int right,
       int (*compare)(const void *key1,const void *key2))
{
    int mid;

    if(left < right)//左边下标大于等于右边小标--递归结束--归并排序完成
    {
        mid = (int)((right - left + 1)/2);//求出中间位置

        if(mgsort(data,size,esize,left,mid,compare) < 0)//归并排序左区间
            return -1;

        if(mgsort(data,size,esize,mid+1,right,compare) < 0)//归并右区间
            return -1;

        if(merge(data,esize,left,mid,right,compare) < 0)//对有序子序列进行归并
            return -1;
    }//data指向已经排好序的集合

    return 0;
}

int main()
{
    return 0;
}
