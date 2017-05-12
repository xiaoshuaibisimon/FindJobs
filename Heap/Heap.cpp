#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef  struct _Heap
{
    int     size;
    void    (*destroy)(void * data);
    int     (*compare)(const void *key1,const void * key2);

    void    **tree;
}Heap;

#define heap_size(heap)     ((heap)->size)

void    heap_init(Heap *heap,int (*compare)(const void *key1,const void * key2),
                void (*destroy)(void * data));

void    heap_destroy(Heap *heap);

int     heap_insert(Heap *heap,const void *data);

int     heap_extract(Heap *heap,void **data);

#define heap_parent(npos)       ((int)(((npos)-1)/2))
#define heap_left(npos)         (((npos)*2)+1)
#define heap_right(npos)         (((npos)*2)+2)

//O(1)
void    heap_init(Heap *heap,int (*compare)(const void *key1,const void * key2),
                void (*destroy)(void * data))
{
    heap->size = 0;
    heap->destroy = destroy;
    heap->compare = compare;
    heap->tree = NULL;//数组初始化为NULL

    return ;
}

//O(n)--如果destroy==NULL，则是O（1）？
void    heap_destroy(Heap *heap)
{
    int i= 0;

    if(heap->destroy != NULL)//销毁数组里面每一个元素对应的二级内存
    {
        for(i = 0;i < heap_size(heap);i++)
        {
            heap->destroy(heap->tree[i]);//每一个元素都是void *--对应二级内存，需要释放
        }
    }

    free(heap->tree);//释放数组--二叉堆存储节点的内存部分

    memset(heap,0,sizeof(Heap));//清空堆结构体

    return ;
}


//O(lgn)
int     heap_insert(Heap *heap,const void *data)
{
    void        *temp = NULL,//用于交换父子节点的时候使用
                **tmp = NULL;//缓存新数组的起始地址
    int         ipos,
                ppos;

    if((tmp = (void **)realloc(heap->tree,(heap_size(heap) + 1) * sizeof(void *))) == NULL)//重新分配内存--存放新节点
        return -1;
    else
        heap->tree = tmp;//更新数组起始地址

    heap->tree[heap_size(heap)] = (void *)data;//将新节点放入数组最后的位置

    ipos = heap_size(heap);//记录新节点的位置--最后一个位置--对应0索引数组
    ppos = heap_parent(ipos);//记录新节点的父节点的位置

    //只要不是空堆，且在大顶堆下，父节点小于子节点--需要交换位置
    while(ipos > 0 && heap->compare(heap->tree[ppos],heap->tree[ipos]) < 0)
    {
        temp = heap->tree[ppos];
        heap->tree[ppos] = heap->tree[ipos];
        heap->tree[ipos] = temp;

        //迭代更新需要交换的节点位置
        ipos = ppos;
        ppos = heap_parent(ipos);
    }

    //更新容量
    heap->size++;

    return 0;
}

//O(lgn)
int     heap_extract(Heap *heap,void **data)
{
    void               *save,
                       *temp;//按理说为了阅读性，应该顶一个另一个void ** 变量缓存新的数组起始地址
                             //但是考虑到指针的本质--都是地址，多少级指针都只是一个机器字长的变量/内存空间，且void*是万能收纳盒
                             //所以可用void*接收void**表示数组的新起始地址，后面赋值的时候进行强制类型转换即可

    int                ipos,
                       lpos,
                       rpos,
                       mpos;

    //不允许操作空堆
    if (heap_size(heap) == 0)
       return -1;

    //保存堆顶元素--数组第一个元素就是要被删除的元素
    *data = heap->tree[0];

    //缓存数组最后一个元素
    save = heap->tree[heap_size(heap) - 1];

    if (heap_size(heap) - 1 > 0)
    {//不止一个元素

       if ((temp = (void **)realloc(heap->tree, (heap_size(heap) - 1) * sizeof
          (void *))) == NULL)
       {//重新分配内存
          return -1;
       }
       else
       {
          heap->tree = (void **)temp;//更新数组起始地址
       }

       //更新容量
       heap->size--;

    }
    else
    {//只有一个元素--该元素也是即将被删除的元素

       //删除最后一个元素--相当于清空堆
       free(heap->tree);//释放数组所占据的内存
       heap->tree = NULL;//避免野指针
       heap->size = 0;//更新容量
       return 0;//删除最后一个元素--直接返回，没有后面的事情了

    }


    //将之前保存的最后一个元素放到堆顶位置
    heap->tree[0] = save;

    //从上往下调整树/堆的结构
    ipos = 0;//起始位置--根节点
    lpos = heap_left(ipos);//左孩子位置
    rpos = heap_right(ipos);//右孩子位置

    while (1) {

       //选择孩子和当前节点进行交换

       lpos = heap_left(ipos);
       rpos = heap_right(ipos);

       if (lpos < heap_size(heap) && heap->compare(heap->tree[lpos], heap->
          tree[ipos]) > 0)
       {//没有遍历完数组，且左孩子大于父节点
          mpos = lpos;//缓存左孩子位置--选择左孩子和父节点交换
       }
       else
       {
          //遍历完数组没有发现孩子比父节点大，
          //或者虽然没有遍历完但左孩子都小于父节点--不进行交换--直到发现右孩子比父节点大
          mpos = ipos;
       }

       if (rpos < heap_size(heap) && heap->compare(heap->tree[rpos], heap->
          tree[mpos]) > 0)
       {//没有遍历完数组，且右孩子大于父节点--到此为止说明，如果左右孩子都大于父节点，优先选择右孩子进行交换
          mpos = rpos;//缓存右孩子位置--选择右孩子和父节点交换
       }



       if (mpos == ipos)
       {//调整完毕--移除结束
          break;
       }
       else
       {


          //将选择好的孩子和父节点进行交换
          temp = heap->tree[mpos];
          heap->tree[mpos] = heap->tree[ipos];
          heap->tree[ipos] = temp;


          //迭代更新 新的父节点 位置--以便往下继续比较父子节点大小
          ipos = mpos;

       }
    }

    //移除完成
    return 0;
}

int main()
{
    return 0;
}
