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

    //需要一个临时变量缓存每一次待插入的元素
    if((key = (char *)malloc(esize)) == NULL)
        return -1;

    //外层循环从下标为1的元素开始--默认下标为0的元素是排好序的
    for(i = 1;i < size;i++)
    {
        j = i - 1;
        memcpy(key,&a[esize*i],esize);//缓存每次待插入的元素
        while(j>=0 && compare(&a[esize*j],key))//将待插入的元素和有序集合里面的元素一一对比
        {
            memcpy(&a[esize*(j+1)],&a[esize*j],esize);//升序排列--前面的元素（已经有序）只要比待插入大就往后挪动一个位置
            j--;//从右往左遍历--默认下标为0开始的是有序的
        }

        memcpy(&a[esize*(j+1)],key,esize);//插入待插入元素--准备从无序集合里选取下一个进行插入
    }

    free(key);//释放缓存节点

    return 0;
}


int main()
{

    return 0;
}
