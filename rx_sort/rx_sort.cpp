#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

int
rxsort(int *data,int size,int p,int k)
{
    int     *counts,
            *temp;

    int     index,
            pval,
            i,
            j,
            n;

    //为计数数组分配空间--每遍历一个有效位需要更新一下该数组
    if((counts = (int*)malloc(k*sizeof(int))) == NULL)
        return -1;

    //为临时有序数组分配空间--每遍历一个有效位需要更新一下该数组
    if((temp = (int *)malloc(size * sizeof(int))) == NULL)
        return -1;

    //从最低有效位到最高有效位
    for(n = 0;n < p;n++)
    {
        for(i = 0;i < k;i++)//先把计数数组初始化为0--针对每一个有效位都要重新初始化
            counts[i] = 0;

        //计算出基数对应于该有效位的分割值
        pval = (int)pow((double)k,(double)n);

        //遍历无序集合--统计该有效位的出现次数
        for(j =0;j < size;j++)
        {
            index = (int)(data[j]/pval) % k;//得到该有效位的数字
            counts[index] += 1;
        }

        //调整计数值
        for(i = 1;i < k;i++)
            counts[i] += counts[i-1];

        //从最后一个元素开始--根据当前有效位放到有序集合
        for(j = size-1;j >= 0;j--)
        {
            index = (int)(data[j]/pval) % k;//当前有效位数字--是计数数组的索引
            temp[counts[index] - 1] = data[j];//根据偏移量（计数数组元素）放置元素
            counts[index]--;//调整计数值
        }

        //将针对当前有效位的阶段有序集合复制到无序数组
        memcpy(data,temp,size*sizeof(int));
    }

    //回收内存
    free(counts);
    free(temp);

    return 0;
}

int main()
{
    return 0;
}
