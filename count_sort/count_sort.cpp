#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//k是集合中最大元素值+1
int
ctsort(int *data,int size,int k)
{
    int     *counts,
            *tmp;

    int     i,
            j;

    if((counts = (int *)calloc(k,sizeof(int))) == NULL)//为计数数组分配内存并初始化为0
        return -1;

    if((tmp = (int*)malloc(sizeof(int)*size)) == NULL)//为整个有序集合分配内存
        return -1;

    for(j = 0;j < size;j++)
    {//遍历无序集合统计各元素出现次数
        counts[data[j]] = counts[data[j]] + 1;
    }

    for(i = 1;i < k;i++)
    {//调整计数值--得到元素在有序集合中的偏移量
        counts[i] += counts[i-1];
    }

    for(j = size -1;j >= 0;j--)
    {//从无序集合中最后一个元素开始将元素放入有序集合
        tmp[counts[data[j]] - 1] = data[j];
        counts[data[j]]--;//调整计数值--保持偏移量同步
    }

    //将有序集合复制到之前的无序集合的内存
    memcpy(data,tmp,size * sizeof(int));

    //回收内存
    free(counts);
    free(tmp);

    return 0;
}

int main()
{
    return 0;
}
