#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

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

    //分配内存
    if((pval = (char *)malloc(esize)) == NULL)
        return -1;

    if((tmp = (char *)malloc(esize)) == NULL)
    {
        free(pval);
        return -1;
    }


    //选取中位数--随机法
    r[0] = (rand()%(right - left + 1)) + left;
    r[1] = (rand()%(right - left + 1)) + left;
    r[2] = (rand()%(right - left + 1)) + left;

    issort(r,3,sizeof(int),compare_int);//进行插入排序得到中位数--实际上是下标
    memcpy(pval,&a[esize*r[1]],esize);//缓存枢纽--分割值

    //做一下调整--否则在do-while里面直接递增/递减或漏掉首尾两端的元素
    left--;
    right++;

    while(1)
    {
        do
        {
            right--;
        }while(compare(&a[esize*right],pval) > 0);//向左找小的元素
        do
        {
            left++;
        }while(compare(&a[esize*left],pval) < 0);//向右找大的元素

        if(left >= right )//左边全是小的元素，右边全是大的--可以一分为二了
            break;
        else
        {//如果还不能一分为二之前，发现位置错误（左边有大元素，右边有小的元素）--交换元素
            memcpy(tmp,&a[left*esize],esize);
            memcpy(&a[left*esize],&a[right*esize],esize);
            memcpy(&a[right*esize],tmp,esize);
        }
    }

    //回收内存
    free(pval);
    free(tmp);

    //返回右边的标志--因为先处理的是右边的下标（先从右往左）
    return right;//分割值下标
}

int
qksort(void *data,int size,int esize,int left,int right,
       int(*compare)(const void *key1,const void *key2))
{
    int     mid;

    //当传进来的 左边下标 大于等于 右边下标--意味着每一个区间都只有一个元素了--结束递归--开始返回
    while(left < right)
    {
        //先找到分割值的下标
        if((mid = partition(data,esize,left,right,compare)) < 0)
            return -1;

        //递归分割左半部分--至此左半部分层层递归求解
        if(qksort(data,size,esize,left,mid,compare) < 0)
            return -1;

        //迭代以后--下次循环开始处理右半部分
        left = mid + 1;
    }

    //当left >= right的时候开始回归--每个区间只有一个元素--结束排序
    return 0;
}
/************************************************************************************/
typedef struct _Directory
{
    char    name[MAXNAMLEN + 1];
}Diretory;

static int
compare_dir(const void *key1,const void * key2)
{
    int retval;

    if((retval = strcmp(((const Diretory *)key1)->name,((const Diretory *)key2)->name)) > 0)
        return 1;
    else if((retval = strcmp(((const Diretory *)key1)->name,((const Diretory *)key2)->name)) < 0)
        return -1;
    else
        return 0;
}

int
directls(const char *path,Diretory **dir)
{

    DIR             *dirptr;
    Diretory        *tmp;
    struct dirent   *curdir;
    int             count,i;


    if((dirptr = opendir(path)) == NULL)//打开目录
        return -1;

    *dir = NULL;
    count = 0;

    while((curdir = readdir(dirptr)) != NULL)//循环读取目录--一次只能返回一个目录文件节点
    {
        count++;

        if((tmp = (Diretory*)realloc(*dir,count*sizeof(Diretory))) == NULL)//重新分配空间
        {
            free(*dir);
            return -1;
        }
        else
        {
            *dir = tmp;//更新存储目录文件名的内存空间的起始地址
        }

        strcpy(((*dir)[count-1]).name,curdir->d_name);//赋值文件名
    }

    //关闭目录
    closedir(dirptr);

    //堆获取的文件名进行排序
    if(qksort(*dir,count,sizeof(Diretory),0,count-1,compare_dir) != 0)
        return -1;

    //打印排好序的目录文件名
    for(i = 0; i < count;i++)
    {
        printf("%s\n",((*dir)[i]).name);
    }

    return count;
}

int main()
{
    Diretory *dir =NULL;
    directls("./",&dir);
    free(dir);//记得回收内存

    return 0;
}
